
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct test {
    double f1;
    long f2;
};

struct test getarg(int, ...);

struct test maketest(long base) {
    struct test val = {.f1 = (double) base, .f2 = base};
    return val;
}

struct test getter(int idx) {
    return getarg(idx, maketest(0), maketest(10), maketest(20), maketest(30), maketest(40), maketest(50), maketest(60),
                  maketest(70), maketest(80), maketest(90));
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = 0; i < 10; i++) {
        struct test val = getter(i);
        ASSERT(val.f1 == (double) i * 10);
        ASSERT(val.f2 == i * 10);
    }
    return EXIT_SUCCESS;
}
