#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a : 7;
    short b : 10;
    int c : 13;
    long d : 15;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -4096; i < 4096; i++) {
        ASSERT(sum(&(struct param){
            .a = 10,
            .b = 100,
            .c = (i % 2048),
            .d = i
        }) == (int) (10 + 100 + (i % 2048) + i + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
