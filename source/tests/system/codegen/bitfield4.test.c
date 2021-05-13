#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct type1 {
    int8_t field1;
    int32_t field2 : 24;
    int8_t field3;
    int32_t field4 : 20;
    int8_t field5;
};

void assign(struct type1 *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int64_t i = -100; i < 100; i++) {
        struct type1 val = {0};
        assign(&val, i);
        ASSERT(val.field1 == 0);
        ASSERT(val.field2 == i);
        ASSERT(val.field3 == 0);
        ASSERT(val.field4 == i + 1);
        ASSERT(val.field5 == 0);
    }
    return EXIT_SUCCESS;
}