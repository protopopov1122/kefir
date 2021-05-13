#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a;
    int b : 8;
    char c;
    long d : 16;
    char : 0;
    long e : 16;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -0x1fff; i < 0x1fff; i++) {
        ASSERT(sum(&(struct param){
            .a = 111,
            .b = -57 + (i & 0x4f),
            .c = 89,
            .d = -0xffe,
            .e = i
        }) == (int) (111 - 57 + (i & 0x4f) + 89 - 0xffe + i + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}