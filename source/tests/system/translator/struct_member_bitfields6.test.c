#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a : 3;
    short b : 14;
    char : 0;
    int c : 20;
    long d : 5;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = 0; i < 100; i++) {
        ASSERT(sum(&(struct param){.a = -3, .b = i, .c = i * 8, .d = 15}) ==
               (int) (-3 + i * 9 + 15 + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
