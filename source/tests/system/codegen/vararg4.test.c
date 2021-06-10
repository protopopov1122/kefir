
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct test {
    long arr[4];
};

struct test getarg(int, ...);

struct test maketest(long base) {
    struct test val = {.arr = {base, base + 1, base + 10, base + 100}};
    return val;
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        struct test arg = getarg(0, maketest(i - 1), maketest(i), maketest(i + 1));
        ASSERT(arg.arr[0] == i - 1);
        ASSERT(arg.arr[1] == i);
        ASSERT(arg.arr[2] == i + 9);
        ASSERT(arg.arr[3] == i + 99);
        arg = getarg(1, maketest(i - 1), maketest(i), maketest(i + 1));
        ASSERT(arg.arr[0] == i);
        ASSERT(arg.arr[1] == i + 1);
        ASSERT(arg.arr[2] == i + 10);
        ASSERT(arg.arr[3] == i + 100);
        arg = getarg(2, maketest(i - 1), maketest(i), maketest(i + 1));
        ASSERT(arg.arr[0] == i + 1);
        ASSERT(arg.arr[1] == i + 2);
        ASSERT(arg.arr[2] == i + 11);
        ASSERT(arg.arr[3] == i + 101);
    }
    return EXIT_SUCCESS;
}
