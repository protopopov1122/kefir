
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

const char *printint(int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        const char *result = printint(i);
        int out = -1000;
        sscanf(result, "INTEGER: %i\n", &out);
        ASSERT(out == i);
    }
    return EXIT_SUCCESS;
}
