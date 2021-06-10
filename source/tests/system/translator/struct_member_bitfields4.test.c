#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a : 7;
    char b : 7;
    int c : 17;
    char d : 6;
    long e : 40;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -5000; i < 5000; i++) {
        ASSERT(sum(&(struct param){.a = -64, .b = 57, .c = i % 1000, .d = -10, .e = i}) ==
               (int) (-64 + 57 + (i % 1000) - 10 + i + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
