#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a : 3;
    char b : 5;
    short c : 7;
    long d : 20;
    char : 0;
    int e : 15;
};

void assign(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -4; i <= 3; i++) {
        struct param val = {0};
        assign(&val, i);
        ASSERT(val.a == i);
        ASSERT(val.b == i + 1);
        ASSERT(val.c == i + 2);
        ASSERT(val.d == i + 3);
        ASSERT(val.e == i + 4);
    }

    return EXIT_SUCCESS;
}
