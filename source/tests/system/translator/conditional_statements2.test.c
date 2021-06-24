#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

double unwrap_double(double *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double d = -100.0; d < 100.0; d += 0.01) {
        double res = unwrap_double(&d);
        if (d < 1.0) {
            ASSERT(DOUBLE_EQUALS(res, 0.0, DOUBLE_EPSILON));
        } else {
            ASSERT(DOUBLE_EQUALS(res, 1.0 / d, DOUBLE_EPSILON));
        }
    }
    ASSERT(DOUBLE_EQUALS(unwrap_double(NULL), -1.0, DOUBLE_EPSILON));
    return EXIT_SUCCESS;
}
