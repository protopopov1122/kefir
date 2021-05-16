#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    int field1 : 10;
    int field2 : 7;
    short field3 : 9;
    char : 0;
    char : 6;
    long field4 : 19;
    int field5 : 15;
};

void modify(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -10; i < 10; i++) {
        for (int j = -5; j < 5; j++) {
            struct param val = {
                .field1 = i * 2,
                .field2 = i,
                .field3 = i,
                .field4 = i * 10,
                .field5 = i * 7
            };
            modify(&val, j);
            ASSERT(val.field1 == i * j * 2);
            ASSERT(val.field2 == i * j);
            ASSERT(val.field3 == i * j);
            ASSERT(val.field4 == i * j * 10);
            ASSERT(val.field5 == i * j * 7);
        }
    }

    return EXIT_SUCCESS;
}
