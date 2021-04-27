#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

float plus(float);
float negate(float);
int logical_negate(float);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -10.0f; i < 10.0f; i += 0.1f) {
        ASSERT(FLOAT_EQUALS(plus(i), i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(negate(i), -i, FLOAT_EPSILON));
        ASSERT(logical_negate(i) == !i);
    }
    return EXIT_SUCCESS;
}