#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

float fneg(float);
double dneg(double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -1000.0f; i < 1000.0f; i += 0.001f) {
        ASSERT(FLOAT_EQUALS(fneg(i), -i, FLOAT_EPSILON));
        ASSERT(DOUBLE_EQUALS(dneg(i), -((double) i), DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
