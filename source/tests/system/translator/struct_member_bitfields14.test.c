#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    long a : 3;
    char b : 3;
    short c : 3;
    long d : 9;
    int e : 17;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -2000; i < 2000; i++) {
        ASSERT(sum(&(struct param){
            .a = 3,
            .b = 2,
            .c = 3,
            .d = 60,
            .e = i
        }) == (int) (3 + 2 +3 + 60 + i + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
