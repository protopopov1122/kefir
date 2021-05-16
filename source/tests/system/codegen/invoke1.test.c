#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int sumseq(int);
double fdbl(double);

int sum10(int a, int b,
          int c, int d,
          int e, int f,
          int g, int h,
          int i, int j) {
    return a + b + c + d + e + f + g + h + i + j;
}

double fsum(double x, double y) {
    return x + y;
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = 0; i < 100; i++) {
        ASSERT(sumseq(i) == (i * 10 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9));
        ASSERT(DOUBLE_EQUALS(fdbl(i * 2.75), i * 5.5, DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
