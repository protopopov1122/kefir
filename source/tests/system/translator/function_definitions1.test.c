#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int sum(int, int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        for (int j = -1000; j < 1000; j++) {
            ASSERT(sum(i, j) == i + j);
        }
    }
    return EXIT_SUCCESS;
}
