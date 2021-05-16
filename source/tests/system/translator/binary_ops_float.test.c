#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

float sum(float, float);
float sub(float, float);
float mul(float, float);
float divide(float, float);
int less(float, float);
int lesseq(float, float);
int greater(float, float);
int greatereq(float, float);
int equals(float, float);
int noteq(float, float);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -10.0f; i < 10.0f; i += 0.1f) {
        for (float j = -10.0f; j < 10.0f; j += 0.1f) {
            ASSERT(FLOAT_EQUALS(sum(i, j), i + j, FLOAT_EPSILON));
            ASSERT(FLOAT_EQUALS(sub(i, j), i - j, FLOAT_EPSILON));
            ASSERT(FLOAT_EQUALS(mul(i, j), i * j, FLOAT_EPSILON));
            ASSERT(FLOAT_EQUALS(divide(i, j), i / j, FLOAT_EPSILON));

            ASSERT(less(i, j) == (i < j));
            ASSERT(lesseq(i, j) == (i <= j));
            ASSERT(greater(i, j) == (i > j));
            ASSERT(greatereq(i, j) == (i >= j));
            ASSERT(equals(i, j) == (i == j));
            ASSERT(noteq(i, j) == (i != j));
        }
    }
    return EXIT_SUCCESS;
}
