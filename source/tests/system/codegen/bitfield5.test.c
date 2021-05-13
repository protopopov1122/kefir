#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct type1 {
    int8_t field1;
    int16_t field2;
    int64_t field3 : 38;
    int16_t field4;
    int64_t field5 : 45;
    int8_t field6;
    int64_t field7 : 51;
    int8_t field8;
};

void assign(struct type1 *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int64_t i = -100; i < 100; i++) {
        struct type1 val = {0};
        assign(&val, i);
        ASSERT(val.field1 == 0);
        ASSERT(val.field2 == 0);
        ASSERT(val.field3 == i);
        ASSERT(val.field4 == 0);
        ASSERT(val.field5 == i + 1);
        ASSERT(val.field6 == 0);
        ASSERT(val.field7 == i + 2);
        ASSERT(val.field8 == 0);
    }
    return EXIT_SUCCESS;
}