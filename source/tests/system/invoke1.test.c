#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

static int sum = 0;

void sumseq(int);

void sum10(int a, int b,
          int c, int d,
          int e, int f,
          int g, int h,
          int i, int j) {
    sum = a + b + c + d + e + f + g + h + i + j;
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = 0; i < 100; i++) {
        sumseq(i);
        ASSERT(sum == (i * 10 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9));
    }
    return EXIT_SUCCESS;
}