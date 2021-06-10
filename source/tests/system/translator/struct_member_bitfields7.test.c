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
    long d : 35;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -1024; i <= 1024; i++) {
        ASSERT(sum(&(struct param){.a = -3, .b = i & 0xff, .c = i, .d = ~i}) ==
               (int) (-3 + (i & 0xff) + i + ~i + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
