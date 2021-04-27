#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int preinc(int *);
int postinc(int *);
int predec(int *);
int postdec(int *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100, a = i, b = i; i < 100; i++) {
        ASSERT(a == i);
        ASSERT(b == i);
        ASSERT(preinc(&a) == i + 1);
        ASSERT(postinc(&b) == i);
        ASSERT(a == i + 1);
        ASSERT(b == i + 1);
    }
    for (int i = 100, a = i, b = i; i > -100; i--) {
        ASSERT(a == i);
        ASSERT(b == i);
        ASSERT(predec(&a) == i - 1);
        ASSERT(postdec(&b) == i);
        ASSERT(a == i - 1);
        ASSERT(b == i - 1);
    }
    return EXIT_SUCCESS;
}