#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a : 7;
    char b : 5;
    long c : 29;
    char d : 6;
    short e : 13;
    int f : 23;
    long g : 35;
    short h : 14;
};

void modify(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -10; i < 10; i++) {
        for (int j = -10; j < 10; j++) {
            struct param val = {
                .a = j, .b = (j + 1) & 15, .c = j + 2, .d = j + 3, .e = j + 4, .f = j + 5, .g = j + 6, .h = j + 7};
            modify(&val, i);
            ASSERT(val.a == j - i);
            ASSERT((val.b & 15) == ((((j + 1) & 15) - i) & 15));
            ASSERT(val.c == j + 2 - i);
            ASSERT(val.d == j + 3 - i);
            ASSERT(val.e == j + 4 - i);
            ASSERT(val.f == j + 5 - i);
            ASSERT(val.g == j + 6 - i);
            ASSERT(val.h == j + 7 - i);
        }
    }

    return EXIT_SUCCESS;
}
