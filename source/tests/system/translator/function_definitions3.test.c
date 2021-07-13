#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    double x;
    double y;
    double z;
};

void sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double x = -10.0; x < 10.0; x += 0.1) {
        for (double y = -10.0; y < 10.0; y += 0.1) {
            struct param p = {.x = x, .y = y, .z = 0};
            sum(&p);
            ASSERT(DOUBLE_EQUALS(p.z, x + y, DOUBLE_EPSILON));
        }
    }
    return EXIT_SUCCESS;
}
