#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int test1(int, int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            ASSERT(test1(i, j) == i + j + 10);
        }
    }
    return EXIT_SUCCESS;
}