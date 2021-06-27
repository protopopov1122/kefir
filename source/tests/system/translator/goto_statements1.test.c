#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

double sumall(double[], unsigned int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double d = -2.5; d <= 2.5; d += 0.1) {
        double result = sumall((double[]){d, d + 10, d * 2, d / 3, d - 1, d * 10, d}, 6);
        ASSERT(DOUBLE_EQUALS(result, d + d + 10 + d * 2 + d / 3 + d - 1 + d * 10, DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
