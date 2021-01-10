#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct aggregate {
    double f1;
    union {
        int32_t f2[2];
        char f3[8];
    };
};

double sumstruct(int64_t x, struct aggregate a) {
    return (a.f1 - x) * (a.f2[0] + a.f2[1]);
}

double proxysum(struct aggregate);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    struct aggregate agg;
    for (int64_t i = -100; i < 100; i++) {
        for (int64_t j = -100; j < 100; j++) {
            agg.f1 = i + j * 0.01;
            agg.f2[0] = i;
            agg.f2[1] = j;
            ASSERT(proxysum(agg) == ((i + j * 0.01) - 5) * (i + j));
        }
    }
    return EXIT_SUCCESS;
}