#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    long a : 33;
    char b : 8;
    char : 0;
    short c : 7;
    long d : 17;
    int e : 22;
    char : 5;
    long f : 39;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -2000; i < 2000; i++) {
        for (int j = -1000; j < 1000; j += 250) {
            ASSERT(sum(&(struct param){.a = 2 * -i, .b = 2, .c = 10, .d = -60, .e = i, .f = j}) ==
                   (int) (2 * -i + 2 + 10 - 60 + i + j + sizeof(struct param) + _Alignof(struct param)));
        }
    }
    return EXIT_SUCCESS;
}
