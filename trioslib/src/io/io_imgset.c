#include <pacbasic.h>
#include <pacio.h>
#include "pacio_local.h"

/* #define _DEBUG_ */


/*
     -------------------------------------------
     FUNCTION: imgset_read
     -------------------------------------------
*/

imgset_t *     /*+ Purpose: read an image collection file                  +*/ 
 imgset_read(
   char *fname   /*+ In: Image collection's file name                      +*/
 )
/*+ Return: a pointer to a image set structure on success, NULL on failure +*/
{
/*  author: Nina S. Tomita (nina@ime.usp.br)                                */
/*  date: Mon Nov 25 1996                                                   */

/*  Modification by:  Nina S. Tomita, R. Hirata Jr. (nina@ime.usp.br)       */
/*  Date: Mon Nov  3 1997                                                   */
/*  Mod: Modified to read the new file format                               */


  char tag, dot ;
  int  tags_read, stop ;

  FILE     *fd ;
  int      ngroups, grpsize ;
  char     name[256] ;
  int      k, i ;
  imgset_t *imgset ;


#ifdef _DEBUG_
pac_debug("It will read %s", fname) ;
#endif

  tags_read = 0 ;
  stop = 0 ;

  if(!(fd = fopen(fname, "r"))) {
    return (imgset_t *)pac_error(1, "File (%s) open failed.", fname) ;
  }
  

  /* read & check file header ------------------------------------------- */
  if(!header_match(fd, "IMGSET  ")) {
    fclose(fd) ;
    return (imgset_t *)pac_error(1, "File header does not match.") ;
  }


  while(!stop)  {
    
    while(((dot=(char)fgetc(fd)) != '.') && (dot!=(char)EOF)) ;

    if(dot==(char)EOF) {
      fclose(fd) ;
      return (imgset_t *)pac_error(1, "Unexpected end of file. No tag found.") ;
    }

    tag = (char)fgetc(fd) ;

    switch (tag) {
      
    case 'n': 
      if(1 != fscanf(fd, "%d", &ngroups)) {
       fclose(fd) ;
       pac_fatal("Unexpected data or end of file") ;
      } 
      tags_read++ ;
      break ;

    case 'f':
      if(1 != fscanf(fd, "%d", &grpsize)) {
        fclose(fd) ;
        pac_fatal("Unexpected data or end of file") ;
      } 
      tags_read++ ;
      break ;

    case 'd' : 
      stop = 1 ;
      break ;

    default : 
      fclose(fd) ;
      (void)pac_error(1,"Unexpected tag %c ",tag) ;
      return (imgset_t *)pac_error(1, " File format error") ;
    }
  }

  if(tags_read!=2) {
    fclose(fd) ;
    return (imgset_t *)pac_error(1,
      "Number of groups or files in the group is missing.") ;
  }

#ifdef _DEBUG_
pac_debug("ngroups=%d  grpsize=%d", ngroups, grpsize);
#endif

  if(!(imgset = imgset_create(ngroups, grpsize))) {
    fclose(fd) ;
    return (imgset_t *)pac_error(MSG, "imgset_read: imgset_create() failed.") ;
  }

  for(i=1; i<=grpsize; i++) {
    if(fscanf(fd, "%s", name) != 1) {
      fclose(fd) ;
      pac_fatal("Unexpected data or end of file") ;
    } 
    if(strcmp(name, "\"\"") == 0) name[0]='\0';
    if(!imgset_set_dname(imgset, i, name)) {
      return
      (imgset_t *)pac_error(MSG, "imgset_read: imgset_set_dname() failed.") ;
    }     
  }
  
  for(k=1; k<=ngroups; k++) {
    for(i=1; i<=grpsize; i++) {
      if(fscanf(fd, "%s", name) != 1) {
	fclose(fd) ;
        pac_fatal("Unexpected data or end of file") ;
      } 
      if(strcmp(name, "\"\"") == 0) name[0]='\0';
      if(!imgset_set_fname(imgset, i, k, name)) {
	return
        (imgset_t *)pac_error(MSG,"imgset_read: imgset_set_fname() failed.") ;
      }  
    }
  }

  fclose(fd) ;
  return(imgset) ;

}


/*
     -------------------------------------------
     FUNCTION: imgset_write
     -------------------------------------------
*/

int            /*+ Purpose: write an image set file               +*/ 
  imgset_write(
    char *fname,       /*+ In: file name                          +*/
    imgset_t *imgset   /*+ In: imgset to be written               +*/
  )
/*+ Return: 1 on success, 0 on failure                            +*/
{
/*  author: Nina S. Tomita  (nina@ime.usp.br)                      */
/*  date: Mon Nov 25 1996                                          */

/*  Modification by:  Nina S. Tomita, R. Hirata Jr. (nina@ime.usp.br)       */
/*  Date: Mon Nov  3 1997                                                   */
/*  Mod: Modified to write the new file format                              */


  header_t header = {"IMGSET  ", ""};
  FILE *fd ;
  int  i, k ;
  char *name ;


  if(!(fd = fopen(fname, "w"))) {
    return pac_error(1, "File (%s) open failed.", fname) ;
  }

  header_write(fd, &header) ;

  fprintf(fd, "%s %d\n", ".n", imgset->ngroups) ;
  fprintf(fd, "%s %d\n", ".f", imgset->grpsize) ;

  fprintf(fd, "%s\n", ".d") ;


  for(i=1; i<=imgset->grpsize; i++) {
    name = imgset_get_dname(imgset, i) ;
    if(name==NULL) {
      fprintf(fd, "%s\n", "\"\"") ;
    }
    else {
      fprintf(fd, "%s\n", name) ;
    }
  }
  fprintf(fd, "\n") ;

  for(k=1; k<=imgset->ngroups; k++) {
    for(i=1; i<=imgset->grpsize; i++) {
      name = imgset_get_fname(imgset, i, k) ;
      if(name==NULL) {
        fprintf(fd, "%s\n", "\"\"") ;
      }
      else {
        fprintf(fd, "%s\n", name) ;
      }
    }
    fprintf(fd, "\n") ;
  }

  fclose(fd) ;

  return(1) ;
}


/*
     -------------------------------------------
     FUNCTION: imgset_create_write
     -------------------------------------------
*/

int            /*+ Purpose: Cretaes and writes an image set file  +*/ 
  imgset_create_write(
    char *fname,       /*+ In: IMGSET file name                   +*/
    char *fname_obs,   /*+ In: observed image file name           +*/
    char *fname_ide,   /*+ In: ideal image file name              +*/
    char *fname_mask   /*+ In: (optional) mask image file name    +*/
  )
/*+ Return: 1 on success, 0 on failure                            +*/
{
/* author: Nina S. Tomita  (nina@ime.usp.br)                       */
/* date: Tue Feb 22 2000                                           */

  imgset_t *imgset ;
  char     dir[256], *fil ;
  int      ngroups, groupsize ;
  unsigned len;


#ifdef _DEBUG_
pac_debug("obs= %s", fname_obs) ;
pac_debug("ide= %s", fname_ide) ;
#endif

  ngroups = 1 ;

  if(fname_mask) groupsize = 3 ;
  else groupsize = 2 ;


  if(!(imgset = (imgset_t *)imgset_create(ngroups, groupsize))) {
    return pac_error(MSG, "imgset_create_write: imgset_create() failed") ;
  }

  /* set observed image */
  fil = strrchr(fname_obs, '/');
  fil++ ;
  len = fil - fname_obs ;
  strncpy(dir, fname_obs, len) ;
  dir[len] = '\0' ;

#ifdef _DEBUG_
  pac_debug("observed file name=%s", fil) ;
  pac_debug("observed directory name=%s", dir) ;
#endif

  if(!imgset_set_dname(imgset, 1, dir)) {
    imgset_free(imgset) ;
    return pac_error(MSG, "imgset_create_write: imgset_create() failed") ;
  }
  if(!imgset_set_fname(imgset, 1, 1, fil)) {
    imgset_free(imgset) ;
    return pac_error(MSG, "imgset_create_write: imgset_create() failed") ;
  }

  /* set ideal image */
  fil = strrchr(fname_ide, '/');
  fil++ ;
  len = fil - fname_ide ;
  strncpy(dir, fname_ide, len) ;
  dir[len] = '\0' ;

#ifdef _DEBUG_
  pac_debug("ideal file name=%s", fil) ;
  pac_debug("ideal directory name=%s", dir) ;
#endif

  if(!imgset_set_dname(imgset, 2, dir)) {
    imgset_free(imgset) ;
    return pac_error(MSG, "imgset_create_write: imgset_create() failed") ;
  }
  if(!imgset_set_fname(imgset, 2, 1, fil)) {
    imgset_free(imgset) ;
    return pac_error(MSG, "imgset_create_write: imgset_create() failed") ;
  }

  if(groupsize == 3) {
    /* set mask image */
    fil = strrchr(fname_mask, '/');
    fil++ ;
    len = fil - fname_mask ;
    strncpy(dir, fname_mask, len) ;
    dir[len] = '\0' ;

#ifdef _DEBUG_
    pac_debug("mask file name=%s", fil) ;
    pac_debug("mask directory name=%s", dir) ;
#endif

    if(!imgset_set_dname(imgset, 3, dir)) {
      imgset_free(imgset) ;
      return pac_error(MSG, "imgset_create_write: imgset_create() failed") ;
    }
    if(!imgset_set_fname(imgset, 3, 1, fil)) {
      imgset_free(imgset) ;
      return pac_error(MSG, "imgset_create_write: imgset_create() failed") ;
    }
  }


  /* write imgset */
  if(!imgset_write(fname, imgset)) {
    imgset_free(imgset) ;
    return pac_error(MSG, "imgset_create_write: imgset_write() failed") ;
  }

  imgset_free(imgset) ;

  return(1) ;

}
