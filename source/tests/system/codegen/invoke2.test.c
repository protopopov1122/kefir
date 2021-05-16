#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct aggregate {
    int64_t array[4];
};

int64_t sumstruct(int64_t x, struct aggregate a) {
    return x * (a.array[0] + a.array[1] + a.array[2] + a.array[3]);
}

int64_t proxysum(struct aggregate);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    struct aggregate agg;
    for (int64_t i = -100; i < 100; i++) {
        for (int64_t j = -100; j < 100; j++) {
            agg.array[0] = i;
            agg.array[1] = j;
            agg.array[2] = i * j;
            agg.array[3] = i ^ j;
            ASSERT(proxysum(agg) == 2 * (i + j + i * j + (i ^ j)));
        }
    }
    return EXIT_SUCCESS;
}
