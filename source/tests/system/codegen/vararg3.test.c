
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

double getarg(int, ...);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(DOUBLE_EQUALS(getarg(0, 0.0), 0.0, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(getarg(0, 1.23456), 1.23456, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(getarg(0, 1.23456, 123456), 1.23456, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(getarg(1, 1.23456, 123456.0), 123456.0, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(getarg(1, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0), 1.0, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(getarg(2, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0), 2.0, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(getarg(7, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0), 7.0, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(getarg(9, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0), 9.0, DOUBLE_EPSILON));
    return EXIT_SUCCESS;
}
