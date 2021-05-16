#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct type1 {
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
};

struct type1 global1;
void fill(uint64_t);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (kefir_int64_t i = -100; i < 100; i++) {
        fill(0xf12345678ul + i);
        ASSERT(global1.u8 == ((uint8_t) 0x78) + i);
        ASSERT(global1.u16 == ((uint16_t) 0x5678) + i);
        ASSERT(global1.u32 == ((uint32_t) 0x12345678) + i);
        ASSERT(global1.u64 == 0xf12345678ul + i);
    }
    return EXIT_SUCCESS;
}
