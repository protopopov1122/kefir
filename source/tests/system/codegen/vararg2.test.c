
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int sumint(int, ...);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(sumint(0) == 0);
    ASSERT(sumint(1, 0) == 0);
    ASSERT(sumint(1, 1) == 1);
    ASSERT(sumint(1, 1, 2, 3) == 1);
    ASSERT(sumint(0, 1, 2, 3) == 0);
    ASSERT(sumint(3, 1, 2, 3) == 6);
    ASSERT(sumint(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10) == 55);
    ASSERT(sumint(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10) == 0);
    ASSERT(sumint(3, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10) == 6);
    return EXIT_SUCCESS;
}