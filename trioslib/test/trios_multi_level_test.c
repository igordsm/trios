#include "minunit.h"
#include <trios.h>

TEST(STRUCTURES_CREATION) {
    int levels[] = {3, 1};
    multi_architecture_t *arch = multi_level_arch_create(2, levels);

    window_t *win = win_create(3, 3, 1);

    multi_level_arch_set_window(arch, 0, 0, 0, win);
    multi_level_arch_set_window(arch, 0, 1, 0, win);
    multi_level_arch_set_window(arch, 0, 2, 0, win);

    multi_level_arch_set_window(arch, 1, 0, 0, win);
    multi_level_arch_set_window(arch, 1, 0, 1, win);
    multi_level_arch_set_window(arch, 1, 0, 2, win);

    mu_assert("Ninputs wrong for level 0", arch->levels[0].ninputs == 1);
    multi_level_arch_free(arch);
    win_free(win);
} TEST_END

TEST(STRUCTURES_CREATION2) {
    int levels[] = {3, 1};
    multi_architecture_t *arch = multi_level_arch_create(2, levels);

    window_t *win = win_create(3, 3, 1);

    multi_level_arch_set_window(arch, 0, 0, 0, win);
    multi_level_arch_set_window(arch, 0, 1, 0, win);
    multi_level_arch_set_window(arch, 0, 2, 0, win);

    multi_level_arch_set_window(arch, 1, 0, 0, win);
    multi_level_arch_set_window(arch, 1, 0, 1, win);
    multi_level_arch_set_window(arch, 1, 0, 2, win);

    multi_level_operator_t *op = multi_level_operator_create(arch);

    multi_level_arch_free(arch);
    multi_level_operator_free(op);
    win_free(win);
} TEST_END

TEST(COLLEC1) {
    int levels[] = {1};
    int i, j;
    multi_architecture_t *arch = multi_level_arch_create(1, levels);

    window_t *win = win_create(3, 3, 1);
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            win_set_point(i, j, 1, 1, win);
        }
    }
    multi_level_arch_set_window(arch, 0, 0, 0, win);

    multi_level_operator_t *op = multi_level_operator_create(arch);
    img_t *input = img_readPGM("./test_img/input1.pgm");
    img_t *ideal = img_readPGM("./test_img/ideal1.pgm");
    xpl_t *xpl = collec_level_operator_bb_main(op, 0, 0, &input, NULL, ideal);

    xpl_write("multi.xpl", xpl, win);

    multi_level_arch_free(arch);
    multi_level_operator_free(op);
    img_free(input);
    img_free(ideal);
    xpl_free(xpl);
    win_free(win);
} TEST_END

TEST(COLLEC2) {
    int levels[] = {2, 1};
    int i, j;
    multi_architecture_t *arch = multi_level_arch_create(2, levels);

    window_t *win = win_create(3, 3, 1);
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            win_set_point(i, j, 1, 1, win);
        }
    }


    multi_level_arch_set_window(arch, 0, 0, 0, win);
    multi_level_arch_set_window(arch, 0, 1, 0, win);

    multi_level_arch_set_window(arch, 1, 0, 0, win);
    multi_level_arch_set_window(arch, 1, 0, 1, win);

    multi_level_operator_t *op = multi_level_operator_create(arch);
    img_t *ideal = img_readPGM("./test_img/ideal1.pgm");

    img_t *inputs[2];
    inputs[0] = img_readPGM("./test_img/input1.pgm");
    inputs[1] = img_readPGM("./test_img/input1.pgm");

    xpl_t *xpl = collec_level_operator_bb_main(op, 1, 0, inputs, NULL, ideal);

    xpl_write("multi2.xpl", xpl, win);

    multi_level_arch_free(arch);
    multi_level_operator_free(op);
    img_free(inputs[0]);
    img_free(inputs[1]);
    img_free(ideal);
    xpl_free(xpl);
    win_free(win);
} TEST_END

TEST(BUILD1) {
    int levels[] = {1};
    int i, j;
    multi_architecture_t *arch = multi_level_arch_create(1, levels);

    window_t *win = win_create(3, 3, 1);
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            win_set_point(i, j, 1, 1, win);
        }
    }
    multi_level_arch_set_window(arch, 0, 0, 0, win);

    imgset_t *set = imgset_create(1, 2);
    imgset_set_dname(set, 1, "./test_img/");
    imgset_set_dname(set, 2, "./test_img/");
    imgset_set_fname(set, 1, 1, "input1.pgm");
    imgset_set_fname(set, 2, 1, "ideal1.pgm");

    multi_level_operator_t *op = multi_level_build(arch, set);

    imgset_free(set);
    multi_level_arch_free(arch);
    multi_level_operator_free(op);
    win_free(win);
} TEST_END


TEST(BUILD2) {
    int levels[] = {2, 1};
    int i, j;
    multi_architecture_t *arch = multi_level_arch_create(2, levels);

    window_t *win1 = win_create(3, 3, 1);
    for (i = 1; i < 3; i++) {
        for (j = 1; j < 3; j++) {
            win_set_point(i, j, 1, 1, win1);
        }
    }
    multi_level_arch_set_window(arch, 0, 0, 0, win1);

    window_t *win2 = win_create(3, 3, 1);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            win_set_point(i, j, 1, 1, win2);
        }
    }
    multi_level_arch_set_window(arch, 0, 1, 0, win2);

    window_t *win3 = win_create(1, 1, 1);
    win_set_point(0, 0, 1, 1, win3);


    multi_level_arch_set_window(arch, 1, 0, 0, win3);
    multi_level_arch_set_window(arch, 1, 0, 1, win3);

    imgset_t *set = imgset_create(1, 2);
    imgset_set_dname(set, 1, "./test_img/");
    imgset_set_dname(set, 2, "./test_img/");
    imgset_set_fname(set, 1, 1, "input1.pgm");
    imgset_set_fname(set, 2, 1, "ideal1.pgm");

    multi_level_operator_t *op = multi_level_build(arch, set);

    imgset_free(set);
    multi_level_arch_free(arch);
    multi_level_operator_free(op);
    win_free(win1);
    win_free(win2);
    win_free(win3);
} TEST_END


TEST(APPLY1) {
    int levels[] = {2, 1};
    int i, j;
    multi_architecture_t *arch = multi_level_arch_create(2, levels);

    window_t *win1 = win_create(3, 3, 1);
    for (i = 1; i < 3; i++) {
        for (j = 1; j < 3; j++) {
            win_set_point(i, j, 1, 1, win1);
        }
    }
    multi_level_arch_set_window(arch, 0, 0, 0, win1);

    window_t *win2 = win_create(3, 3, 1);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            win_set_point(i, j, 1, 1, win2);
        }
    }
    multi_level_arch_set_window(arch, 0, 1, 0, win2);

    window_t *win3 = win_create(1, 1, 1);
    win_set_point(0, 0, 1, 1, win3);


    multi_level_arch_set_window(arch, 1, 0, 0, win3);
    multi_level_arch_set_window(arch, 1, 0, 1, win3);

    imgset_t *set = imgset_create(1, 2);
    imgset_set_dname(set, 1, "./test_img/");
    imgset_set_dname(set, 2, "./test_img/");
    imgset_set_fname(set, 1, 1, "input1.pgm");
    imgset_set_fname(set, 2, 1, "ideal1.pgm");

    multi_level_operator_t *op = multi_level_build(arch, set);
    img_t *input = img_readPGM("test_img/input1.pgm");
    img_t *result = multi_level_apply(op, input);
    img_writePGM("result.pgm", result);

    imgset_free(set);
    img_free(input);
    img_free(result);
    multi_level_arch_free(arch);
    multi_level_operator_free(op);
    win_free(win1);
    win_free(win2);
    win_free(win3);
} TEST_END
#include "runner.h"

