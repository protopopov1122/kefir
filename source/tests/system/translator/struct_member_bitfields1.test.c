#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    signed char field1 : 4;
    unsigned long long field2 : 2;
    signed long field3 : 10;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -512; i < 512; i++) {
        for (int j = -8; j < 8; j++) {
            for (int k = 0; k < 4; k++) {
                struct param p = {.field1 = j, .field2 = k, .field3 = i};
                ASSERT(sum(&p) == j + k + i);
            }
        }
    }
    return EXIT_SUCCESS;
}
