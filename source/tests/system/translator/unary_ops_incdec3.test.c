#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

float preinc(float *);
float postinc(float *);
float predec(float *);
float postdec(float *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -10.0f; i < 10.0; i += 0.1f) {
        float a = i, b = i;
        ASSERT(FLOAT_EQUALS(a, i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(b, i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(preinc(&a), i + 1, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(postinc(&b), i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(a, i + 1, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(b, i + 1, FLOAT_EPSILON));
    }
    for (float i = -10.0f; i < 10.0; i += 0.1f) {
        float a = i, b = i;
        ASSERT(FLOAT_EQUALS(a, i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(b, i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(predec(&a), i - 1, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(postdec(&b), i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(a, i - 1, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(b, i - 1, FLOAT_EPSILON));
    }
    return EXIT_SUCCESS;
}