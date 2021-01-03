#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

extern int8_t int8_1, int8_2;
extern int16_t int16_1, int16_2;
extern int32_t int32_1, int32_2;
extern int64_t int64_1, int64_2;
extern float float32_1, float32_2;
extern double float64_1, float64_2;

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(int8_1 == 104);
    ASSERT(int8_2 == 0);
    ASSERT(int16_1 == 2589);
    ASSERT(int16_2 == 0);
    ASSERT(int32_1 == 0x1ffff);
    ASSERT(int32_2 == 0);
    ASSERT(int64_1 == 0x1ffffffff);
    ASSERT(int64_2 == 0);
    ASSERT(FLOAT_EQUALS(float32_1, 3.14159f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(float32_2, 0.0f, FLOAT_EPSILON));
    ASSERT(DOUBLE_EQUALS(float64_1, 2.718281828, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(float64_2, 0.0, DOUBLE_EPSILON));
    return EXIT_SUCCESS;
}