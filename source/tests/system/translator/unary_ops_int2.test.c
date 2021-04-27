#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

unsigned int plus(unsigned int);
unsigned int negate(unsigned int);
unsigned int invert(unsigned int);
unsigned int logical_negate(unsigned int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (unsigned int i = 0; i < 200; i++) {
        ASSERT(plus(i) == i);
        ASSERT(negate(i) == -i);
        ASSERT(invert(i) == ~i);
        ASSERT(logical_negate(i) == !i);
    }
    return EXIT_SUCCESS;
}