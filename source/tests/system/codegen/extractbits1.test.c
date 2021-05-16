#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

unsigned long extractbits(unsigned long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    ASSERT(extractbits(0xcafebabeu) == 0xe + 0xb + 0xa + 0xb + 0xe + 0xf + 0xa + 0xc);
    for (unsigned int i = 0; i <= 0xf; i++) {
        for (unsigned int j = 0; j <= 0xf; j++) {
            for (unsigned int k = 0; k <= 0xf; k++) {
                for (unsigned int a = 0; a <= 0xf; a++) {
                    ASSERT(extractbits((i << 24) | (j << 16) | (k << 8) | a) == i + j + k + a);
                }
            }
        }    
    }
    return EXIT_SUCCESS;
}
