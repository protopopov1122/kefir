#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    unsigned char arg1;
    unsigned char arg2;
    int result;
};

void char_sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (char i = CHAR_MIN; i < CHAR_MAX; i++) {
        for (char j = CHAR_MIN; j < CHAR_MAX; j++) {
            struct param val = {.arg1 = i, .arg2 = j, .result = 0};
            char_sum(&val);
            ASSERT(val.result == i + j);
        }
    }

    return EXIT_SUCCESS;
}
