#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

typedef struct matrix {
    int table[5][5];
    unsigned dim;
} matrix_t;

extern matrix_t matrix1;
extern matrix_t *(*matrices[])();
matrix_t *get_matrix2();
matrix_t *get_matrix3();
matrix_t *get_matrix4();

void assert_matrix(struct matrix *matrix, int offset) {
    ASSERT(matrix->dim == (unsigned) (5 + offset));
    ASSERT(matrix->table[0][0] == -1 + offset);
    ASSERT(matrix->table[0][1] == 1 + offset);
    ASSERT(matrix->table[0][2] == 2 + offset);
    ASSERT(matrix->table[0][3] == 3 + offset);
    ASSERT(matrix->table[0][4] == 4 + offset);
    ASSERT(matrix->table[1][0] == 0);
    ASSERT(matrix->table[1][1] == 0);
    ASSERT(matrix->table[1][2] == 0);
    ASSERT(matrix->table[1][3] == 0);
    ASSERT(matrix->table[1][4] == 0);
    ASSERT(matrix->table[2][0] == 10 + offset);
    ASSERT(matrix->table[2][1] == 11 + offset);
    ASSERT(matrix->table[2][2] == 12 + offset);
    ASSERT(matrix->table[2][3] == 13 + offset);
    ASSERT(matrix->table[2][4] == 14 + offset);
    ASSERT(matrix->table[3][0] == 15 + offset);
    ASSERT(matrix->table[3][1] == 16 + offset);
    ASSERT(matrix->table[3][2] == 17 + offset);
    ASSERT(matrix->table[3][3] == 18 + offset);
    ASSERT(matrix->table[3][4] == 19 + offset);
    ASSERT(matrix->table[4][0] == 20 + offset);
    ASSERT(matrix->table[4][1] == 21 + offset);
    ASSERT(matrix->table[4][2] == 22 + offset);
    ASSERT(matrix->table[4][3] == 23 + offset);
    ASSERT(matrix->table[4][4] == 24 + offset);
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    assert_matrix(&matrix1, 0);
    assert_matrix(matrices[0](), 10);
    assert_matrix(matrices[1](), 100);
    assert_matrix(matrices[2](), -1);
    return EXIT_SUCCESS;
}
