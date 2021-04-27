#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int plus(int);
int negate(int);
int invert(int);
int logical_negate(int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        ASSERT(plus(i) == i);
        ASSERT(negate(i) == -i);
        ASSERT(invert(i) == ~i);
        ASSERT(logical_negate(i) == !i);
    }
    return EXIT_SUCCESS;
}