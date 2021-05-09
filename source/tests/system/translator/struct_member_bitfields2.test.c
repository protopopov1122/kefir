#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char f1 : 5;
    char f2 : 5;
    char : 0;
    char f3 : 2;
    char : 3;
    int f4 : 10;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    ASSERT(sum(&(struct param){
        .f1 = 14,
        .f2 = -10,
        .f3 = 1,
        .f4 = 500
    }) == 505 + sizeof(struct param) + _Alignof(struct param));
    return EXIT_SUCCESS;
}