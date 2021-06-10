#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a;
    char b : 5;
    short c : 3;
    char : 0;
    int d : 17;
    long e : 13;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -1000; i < 1000; i++) {
        ASSERT(sum(&(struct param){.a = -100, .b = i & 0xf, .c = 2, .d = -0x1f, .e = i}) ==
               (int) (-100 + (i & 0xf) + 2 - 0x1f + i + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
