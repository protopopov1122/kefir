#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long floatToLong(float);
long doubleToLong(double);
float longToFloat(long);
double longToDouble(long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -1000.0f; i < 1000.0f; i += 0.01) {
        ASSERT(floatToLong(i) == ((long) i));
        ASSERT(doubleToLong((double) i) == ((long) i));
    }
    for (long i = -1000; i < 1000; i++) {
        ASSERT(FLOAT_EQUALS(longToFloat((long) i), (float) i, FLOAT_EPSILON));
        ASSERT(DOUBLE_EQUALS(longToDouble((long) i), (double) i, DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}