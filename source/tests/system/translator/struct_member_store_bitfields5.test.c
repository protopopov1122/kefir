#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    long field1 : 9;
    short field2 : 12;
    int field3 : 20;
    char : 0;
    int : 14;
    long field4 : 24;
    char : 6;
    char field5 : 7;
};

void modify(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    for (int i = -10000; i < 10000; i += 50) {
        for (int j = -50; j < 50; j++) {
            struct param val = {
                .field1 = 110,
                .field2 = 1020,
                .field3 = i / 2,
                .field4 = i,
                .field5 = 0
            };
            modify(&val, j);
            ASSERT(val.field1 == 110 + j);
            ASSERT(val.field2 == 1020 + j);
            ASSERT(val.field3 == i / 2 + j);
            ASSERT(val.field4 == i + j);
            ASSERT(val.field5 == j);
        }
    }

    return EXIT_SUCCESS;
}