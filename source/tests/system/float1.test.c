#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct circle_res {
    float length;
    float area;
    float neg_radius;
};

struct circle_res circle(float);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = 0.0f; i < 1000.0f; i += 0.01f) {
        struct circle_res res = circle(i);
        ASSERT(FLOAT_EQUALS(res.length, 2.0f * 3.14159f * i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(res.area, 3.14159f * ((float) (i * i)) / 2.0f, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(res.neg_radius, -i, FLOAT_EPSILON));
    }
    return EXIT_SUCCESS;
}