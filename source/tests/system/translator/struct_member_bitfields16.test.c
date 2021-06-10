#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    int a : 10;
    long : 10;
    int b : 10;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -256; i <= 256; i++) {
        for (int j = -256; j <= 256; j++) {
            struct param val = {.a = i, .b = j};
            ASSERT(sum(&val) == (int) (i + j + sizeof(struct param) + _Alignof(struct param)));
        }
    }
    return EXIT_SUCCESS;
}
