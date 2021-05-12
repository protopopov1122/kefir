#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct type1 {
    uint8_t field1;
    uint64_t field2 : 20;
    uint16_t field3;
    uint64_t field4 : 30;
    uint16_t field5;
    uint8_t field6;
    uint64_t field7 : 40;
    uint8_t field8;
};

long sum(struct type1 *);

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define SIGN(x) ((x) < 0 ? -1 : 1)

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int64_t i = -100; i < 100; i++) {
        struct type1 val = {
            .field1 = 1,
            .field2 = i,
            .field3 = 2,
            .field4 = (ABS(i) << 1) * SIGN(i),
            .field5 = 3,
            .field6 = 4,
            .field7 = (ABS(i) << 2) * SIGN(i),
            .field8 = 5
        };
        long res = sum(&val);
        ASSERT(res == i + (ABS(i) << 1) * SIGN(i) + (ABS(i) << 2) * SIGN(i));
    }
    return EXIT_SUCCESS;
}