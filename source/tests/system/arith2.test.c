#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long arith2(long, long, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -10; i < 10; i++) {
        for (int j = -10; j < 10; j++) {
            for (int k = -100; k < 100; k++) {
                ASSERT(arith2(i, j, k) == (i & ((j | ((~k << 3) ^ 5)) >> 2)));
            }
        }
    }
    return EXIT_SUCCESS;
}