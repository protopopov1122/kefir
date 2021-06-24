#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int char_to_int(char);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (char c = CHAR_MIN; c < CHAR_MAX; c++) {
        if (c >= '0' && c <= '9') {
            ASSERT(char_to_int(c) == c - '0');
        } else {
            ASSERT(char_to_int(c) == -1);
        }
    }
    return EXIT_SUCCESS;
}
