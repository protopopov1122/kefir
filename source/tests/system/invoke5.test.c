
#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct aggregate {
    int64_t f1;
    double f2;
};

struct aggregate addstruct(int64_t x, struct aggregate a) {
    a.f1 += x;
    a.f2 *= x;
    return a;
}

struct aggregate proxyadd(struct aggregate);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int64_t i = -100; i < 100; i++) {
        struct aggregate agg = {
            .f1 = i,
            .f2 = i * 35.46 + 0.05
        };
        struct aggregate res = proxyadd(agg);
        ASSERT(res.f1 == i + 7);
        ASSERT(DOUBLE_EQUALS((i * 35.46 + 0.05) * 7, res.f2, DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}