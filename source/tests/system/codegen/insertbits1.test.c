#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

unsigned long insertbits(char, char, char, char,
                         char, char, char, char);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    for (unsigned int i = 0; i < 0x100; i++) {
        ASSERT(insertbits((i >> 7) & 1,
            (i >> 6) & 1,
            (i >> 5) & 1,
            (i >> 4) & 1,
            (i >> 3) & 1,
            (i >> 2) & 1,
            (i >> 1) & 1,
            (i >> 0) & 1) == i);
    }
    return EXIT_SUCCESS;
}