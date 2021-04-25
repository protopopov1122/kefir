
#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct aggregate {
    int64_t array[4];
};

struct aggregate addstruct(int64_t x, struct aggregate a) {
    a.array[0] += x;
    a.array[1] += x;
    a.array[2] += x;
    a.array[3] += x;
    return a;
}

struct aggregate proxyadd(struct aggregate);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int64_t i = -100; i < 100; i++) {
        struct aggregate agg = {
            .array = {
                i, i * 10, -i, i * (-10)
            }
        };
        struct aggregate res = proxyadd(agg);
        ASSERT(res.array[0] == i + 3);
        ASSERT(res.array[1] == i * 10 + 3);
        ASSERT(res.array[2] == -i + 3);
        ASSERT(res.array[3] == -i * 10 + 3);
    }
    return EXIT_SUCCESS;
}