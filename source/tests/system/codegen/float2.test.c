#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct circle_res {
    double length;
    double area;
    double neg_radius;
};

struct circle_res circle(double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double i = 0.0; i < 1000.0; i += 0.01) {
        struct circle_res res = circle(i);
        ASSERT(DOUBLE_EQUALS(res.length, 2.0 * 3.14159 * i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(res.area, 3.14159 * ((double) (i * i)) / 2.0, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(res.neg_radius, -i, DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
