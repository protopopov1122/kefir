#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long arith1(long, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        for (int j = -100; j < 100; j++) {
            ASSERT(arith1(i, j) == -(((i + j + 10l) * 3l) % 157l));
        }
    }
    return EXIT_SUCCESS;
}