#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a;
    char b : 5;
    short c : 3;
    int d : 17;
    long e : 30;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -4096; i <= 4096; i++) {
        ASSERT(sum(&(struct param){
            .a = -100,
            .b = 11,
            .c = 2,
            .d = -i,
            .e = (~i) & 0xffffff
        }) == (int) (-100 + 11 + 2 - i + ((~i) & 0xffffff) + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
