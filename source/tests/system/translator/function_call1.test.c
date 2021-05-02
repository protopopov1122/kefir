#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long sumall(int count, ...) {
    va_list args;
    va_start(args, count);
    long sum = 0;
    while (count--) {
        long arg = va_arg(args, long);
        sum += arg;
    }
    va_end(args);
    return sum;
}

long sum1(long);
long sum3(long);
long sum5(long);
long sum10(long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (long i = -1000; i < 1000; i++) {
        ASSERT(sum1(i) == i);
        ASSERT(sum3(i) == 3 * i + 3);
        ASSERT(sum5(i) == 5 * i + 10);
        ASSERT(sum10(i) == 10 * i + 45);
    }
    return EXIT_SUCCESS;
}