#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct type1 {
    uint8_t u8;
    int8_t i8;
    uint16_t u16;
    int16_t i16;
    uint32_t u32;
    int32_t i32;
    int64_t i64;

    int64_t result;
};

const int64_t *sum(struct type1 *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int64_t i = -100; i < 100; i++) {
        struct type1 val = {
            .u8 = (uint8_t) (i + 100),
            .i8 = i,
            .u16 = (uint16_t) (i + 1000),
            .i16 = (int16_t) (-i * 10),
            .u32 = (uint32_t) (i + 0xffff),
            .i32 = i * 1000,
            .i64 = i + 0xffffffffl
        };
        const int64_t *res = sum(&val);
        ASSERT(*res == i + 100 + i + i + 1000 + (-i * 10) + i + 0xffff + i * 1000 + i + 0xffffffffl);
        ASSERT(*res == val.result);
        ASSERT(res == &val.result);
    }
    return EXIT_SUCCESS;
}
