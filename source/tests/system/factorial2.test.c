#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int factorial(int);

static long factorial2(int num) {
    if (num < 2) {
        return 1;
    } else {
        return num * factorial2(num - 1);
    }
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (long i = -10; i < 10; i++) {
        ASSERT(factorial(i) == factorial2(i));
    }
    return EXIT_SUCCESS;
}