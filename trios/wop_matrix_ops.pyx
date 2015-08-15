# cython: profile=True
# filename: wop_matrix_ops.pyx


from __future__ import print_function


cimport numpy as np
import numpy as np
cimport cython

import scipy as sp

from cython.parallel import prange

import sys

@cython.boundscheck(False)
@cython.nonecheck(False)
cpdef process_image(dict dataset, np.ndarray[unsigned char, ndim=2] win, np.ndarray[unsigned char, ndim=2] iinput, np.ndarray[unsigned char, ndim=2] output, np.ndarray[unsigned char, ndim=2] mask, extractor):
    cdef int h = iinput.shape[0]
    cdef int w = iinput.shape[1]
    
    cdef int i
    cdef int j
    cdef int l
    cdef int m
    cdef int hh = win.shape[0]
    cdef int ww = win.shape[1]
    cdef int hh2 = hh/2
    cdef int ww2 = ww/2
    cdef int count = 0
    
    wpat = extractor.temp_feature_vector()
    for i in range(hh2, h-hh2):
        for j in range(ww2, w-ww2):
            if (not mask is None) and mask[i,j] > 0:
                count += 1
                
                extractor.extract(iinput, i, j, wpat)
                wpatt = tuple(wpat)
                if not wpatt in dataset:
                    dataset[wpatt] = {}
                if not output[i,j] in dataset[wpatt]:
                    dataset[wpatt][output[i,j]] = 1
                else:
                    dataset[wpatt][output[i,j]] += 1

ctypedef fused raw_data:
    cython.uchar
    cython.uint
    cython.float


@cython.boundscheck(False)
@cython.nonecheck(False)
cpdef long process_one_image(unsigned char[:,:] win, unsigned char[:,:] inp, unsigned char[:,:] out, unsigned char[:,:] msk, raw_data [:,:] X, unsigned char[:] y, extractor, raw_data [:] temp, int k):
    cdef int h, w, i, j, l, ww, hh, ww2, hh2
    hh = win.shape[0]; ww = win.shape[1]
    hh2 = hh/2
    ww2 = ww/2
    
    h = inp.shape[0] ; w = inp.shape[1]
    for i in range(hh2, h-hh2):
        for j in range(ww2, w-ww2):
            if (not msk is None) and msk[i,j] > 0:
                extractor.extract(inp, i, j, temp)
                for l in range(temp.shape[0]):
                    X[k, l] = temp[l]
                y[k] = out[i, j]
                k += 1 
    return k
    

@cython.boundscheck(False)
@cython.nonecheck(False)
cpdef process_image_ordered(imageset, extractor):
    # count all pixels in mask
    cdef long npixels = 0
    cdef long k
    cdef int h, w, i, j, l, ww, hh, ww2, hh2
    cdef unsigned char [:,:] msk
    cdef unsigned char [:,:] inp
    cdef unsigned char [:,:] out
    
    cdef np.ndarray X
    cdef np.ndarray temp
    
    for _i, _o, m in imageset:
        msk = sp.ndimage.imread(m, mode='L')
        for i in range(msk.shape[0]):
            for j in range(msk.shape[1]):
                if msk[i, j] != 0:
                    npixels += 1
                    
    temp = extractor.temp_feature_vector()
    X = np.zeros((npixels, len(extractor)), temp.dtype)
    y = np.zeros(npixels, np.uint8)
    
    k = 0
    for (inp_, out_, msk_) in imageset:
        inp = sp.ndimage.imread(inp_, mode='L')
        out = sp.ndimage.imread(out_, mode='L')
        msk = sp.ndimage.imread(msk_, mode='L')
        if X.dtype == np.uint8:
            k = process_one_image[cython.uchar](extractor.window, inp, out, msk, X, y, extractor, temp, k)
        elif X.dtype == np.float:
            k = process_one_image[cython.float](extractor.window, inp, out, msk, X, y, extractor, temp, k)
        elif X.dtype == np.uint32:
            k = process_one_image[cython.uint](extractor.window, inp, out, msk, X, y, extractor, temp, k)
    
    return X, y

@cython.boundscheck(False)
@cython.nonecheck(False)
cpdef unsigned char[:,:] apply_loop(np.ndarray[unsigned char, ndim=2] window, np.ndarray[unsigned char, ndim=2] image, np.ndarray[unsigned char, ndim=2] mask, classifier, extractor):
    cdef int h = image.shape[0]
    cdef int w = image.shape[1]    
    cdef int wh = int(window.shape[0]/2)
    cdef int ww = int(window.shape[1]/2)
    cdef np.ndarray[unsigned char, ndim=2] output = np.zeros((image.shape[0], image.shape[1]), dtype=np.uint8)
    cdef np.ndarray pat = extractor.temp_feature_vector()
    
    cdef int i, j
    cdef int count = 0
    for i in range(wh, h-wh):
        for j in range(ww, w-ww):
            if mask[i,j] > 0: 
                count += 1
                extractor.extract(image, i, j, pat)
                output[i,j] = classifier.apply(pat)
    return output
    
@cython.boundscheck(False)
@cython.nonecheck(False)
cpdef compare_images(unsigned char[:,:] out, unsigned char[:,:] msk, unsigned char[:,:] res, int x_border, int y_border):
    cdef int i, j
    cdef int w = out.shape[1]
    cdef int h = out.shape[0]

    cdef unsigned long nerrs = 0, npixels = 0
    
    for i in range(y_border, h - y_border):
        for j in range(x_border, w - x_border):
            if msk[i,j] != 0:
                npixels += 1
                if out[i, j] != res[i, j]:
                    nerrs += 1
    
    return (nerrs, npixels)