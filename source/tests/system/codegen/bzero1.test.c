#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

void fillzeros(int *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    int values[8] = {
        1, 2, 3, 4,
        5, 6, 7, 8
    };
    fillzeros(values);

    for (kefir_size_t i = 0; i < 8; i++) {
        REQUIRE(values[i] == 0, KEFIR_INTERNAL_ERROR);
    }
    return EXIT_SUCCESS;
}