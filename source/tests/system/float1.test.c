#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

float circle(float);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = 0.0f; i < 10000.0f; i += 0.01f) {
        ASSERT(FLOAT_EQUALS(circle(i), 2.0f * 3.14159f * i, FLOAT_EPSILON));
    }
    return EXIT_SUCCESS;
}