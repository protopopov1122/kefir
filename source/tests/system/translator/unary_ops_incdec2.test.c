#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

unsigned int preinc(unsigned int *);
unsigned int postinc(unsigned int *);
unsigned int predec(unsigned int *);
unsigned int postdec(unsigned *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (unsigned int i = 0, a = i, b = i; i < 200; i++) {
        ASSERT(a == i);
        ASSERT(b == i);
        ASSERT(preinc(&a) == i + 1);
        ASSERT(postinc(&b) == i);
        ASSERT(a == i + 1);
        ASSERT(b == i + 1);
    }
    for (unsigned int i = 200, a = i, b = i; i > 0; i--) {
        ASSERT(a == i);
        ASSERT(b == i);
        ASSERT(predec(&a) == i - 1);
        ASSERT(postdec(&b) == i);
        ASSERT(a == i - 1);
        ASSERT(b == i - 1);
    }
    return EXIT_SUCCESS;
}
