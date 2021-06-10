#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

void fillzeros(long *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (kefir_size_t i = 1; i < 1000; i++) {
        long l = i;
        fillzeros(&l);
        REQUIRE(l == 0, KEFIR_INTERNAL_ERROR);
    }
    return EXIT_SUCCESS;
}
