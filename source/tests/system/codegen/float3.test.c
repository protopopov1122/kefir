#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

bool fequals(float, float);
bool fgreater(float, float);
bool flesser(float, float);
bool dequals(double, double);
bool dgreater(double, double);
bool dlesser(double, double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -100.0f; i < 100.0f; i += 0.001f) {
        ASSERT(fequals(i, i));
        ASSERT(!fequals(i, i * 2));
        ASSERT(!fequals(i, i + 1));
        ASSERT(!fequals(i, i + FLOAT_EPSILON));

        ASSERT(fgreater(i + FLOAT_EPSILON, i));
        ASSERT(fgreater(fabs(i) * 2, fabs(i)));
        ASSERT(!fgreater(i, i));
        ASSERT(!fgreater(i - FLOAT_EPSILON, i));

        ASSERT(!flesser(i + FLOAT_EPSILON, i));
        ASSERT(!flesser(fabs(i) * 2, fabs(i)));
        ASSERT(!flesser(i, i));
        ASSERT(flesser(i - FLOAT_EPSILON, i));
        ASSERT(flesser(fabs(i) * 2, fabs(i) * 10));
    }
    for (double i = -100.0; i < 100.0; i += 0.001) {
        ASSERT(dequals(i, i));
        ASSERT(!dequals(i, i * 2));
        ASSERT(!dequals(i, i + 1));
        ASSERT(!dequals(i, i + DOUBLE_EPSILON));

        ASSERT(dgreater(i + DOUBLE_EPSILON, i));
        ASSERT(dgreater(fabs(i) * 2, fabs(i)));
        ASSERT(!dgreater(i, i));
        ASSERT(!dgreater(i - DOUBLE_EPSILON, i));

        ASSERT(!dlesser(i + DOUBLE_EPSILON, i));
        ASSERT(!dlesser(fabs(i) * 2, fabs(i)));
        ASSERT(!dlesser(i, i));
        ASSERT(dlesser(i - DOUBLE_EPSILON, i));
        ASSERT(dlesser(fabs(i) * 2, fabs(i) * 10));
    }
    return EXIT_SUCCESS;
}
