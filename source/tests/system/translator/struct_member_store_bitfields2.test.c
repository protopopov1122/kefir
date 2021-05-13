#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    unsigned int x : 11;
    unsigned int : 5;
    unsigned int y : 9;
    unsigned char z : 7;
    unsigned long : 19;
    unsigned char w : 5;
    unsigned long k : 39;
};

void assign(struct param *, unsigned long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (unsigned long i = 0; i < 510; i++) {
        struct param val = {0};
        assign(&val, i);
        ASSERT(val.x == i);
        ASSERT(val.y == i + 1);
        ASSERT(val.z == ((i + 2) & 127));
        ASSERT(val.w == ((i + 3) & 31));
        ASSERT(val.k == i + 4);
    }

    return EXIT_SUCCESS;
}