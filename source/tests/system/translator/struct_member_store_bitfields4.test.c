#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    long field1 : 15;
    short field2 : 11;
    int : 9;
    int field3 : 17;
    long field4 : 20;
    char : 0;
    long field5 : 33;
};

void modify(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -200; i < 200; i++) {
        for (int j = -100; j < 100; j++) {
            if (j == 0) {
                continue;
            }
            struct param val = {
                .field1 = 100 + i, .field2 = 321 + i, .field3 = 786 + i, .field4 = 912 + i, .field5 = 542018 + i};
            modify(&val, j);
            ASSERT(val.field1 == (100 + i) / j);
            ASSERT(val.field2 == (321 + i) / j);
            ASSERT(val.field3 == (786 + i) / j);
            ASSERT(val.field4 == (912 + i) / j);
            ASSERT(val.field5 == (542018 + i) / j);
        }
    }

    return EXIT_SUCCESS;
}
