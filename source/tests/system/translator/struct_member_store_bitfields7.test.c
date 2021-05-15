#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    long a : 25;
    int b : 21;
    short c : 15;
    char d : 7;
    int e;
    char : 6;
    long f : 40;
    int g : 30;
    char h : 8;
    short i : 14;
};

void modify(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -100; i < 100; i++) {
        for (int j = 1; j < 100; j++) {
            struct param val = {
                .a = i * 10,
                .b = i,
                .c = i + 1,
                .d = i / 2,
                .e = i * 20,
                .f = i * 25,
                .g = i * 15,
                .h = i / 2,
                .i = i - 1
            };
            modify(&val, j);
            ASSERT(val.a == (i * 10) % j);
            ASSERT(val.b == i % j);
            ASSERT(val.c == (i + 1) % j);
            ASSERT(val.d == (i / 2) % j);
            ASSERT(val.e == (i * 20) % j);
            ASSERT(val.f == (i * 25) % j);
            ASSERT(val.g == (i * 15) % j);
            ASSERT(val.h == (i / 2) % j);
            ASSERT(val.i == (i - 1) % j);
        }
    }

    return EXIT_SUCCESS;
}