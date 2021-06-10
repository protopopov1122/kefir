#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    long a : 31;
    long b : 33;
    long c : 31;
    long : 0;
    long d : 33;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -0x1fff; i < 0x1fff; i++) {
        ASSERT(sum(&(struct param){.a = i, .b = ~i, .c = -i, .d = i * 2}) ==
               (int) (i + (~i) - i + i * 2 + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
