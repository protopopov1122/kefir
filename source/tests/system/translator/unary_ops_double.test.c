#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

double plus(double);
double negate(double);
int logical_negate(double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double i = -10.0f; i < 10.0f; i += 0.1f) {
        ASSERT(DOUBLE_EQUALS(plus(i), i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(negate(i), -i, DOUBLE_EPSILON));
        ASSERT(logical_negate(i) == !i);
    }
    return EXIT_SUCCESS;
}
