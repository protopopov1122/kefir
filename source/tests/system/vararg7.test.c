
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int getarg(int, ...);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(getarg(0, 0) == 0);
    ASSERT(getarg(0, 0, 1) == 0);
    ASSERT(getarg(1, 0, 10) == 10);
    for (int i = 0; i < 10; i++) {
        ASSERT(getarg(i, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90) == i * 10);
    }
    return EXIT_SUCCESS;
}