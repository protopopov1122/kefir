#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

double preinc(double *);
double postinc(double *);
double predec(double *);
double postdec(double *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double i = -10.0f; i < 10.0; i += 0.1f) {
        double a = i, b = i;
        ASSERT(DOUBLE_EQUALS(a, i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(b, i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(preinc(&a), i + 1, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(postinc(&b), i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(a, i + 1, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(b, i + 1, DOUBLE_EPSILON));
    }
    for (double i = -10.0f; i < 10.0; i += 0.1f) {
        double a = i, b = i;
        ASSERT(DOUBLE_EQUALS(a, i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(b, i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(predec(&a), i - 1, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(postdec(&b), i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(a, i - 1, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(b, i - 1, DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
