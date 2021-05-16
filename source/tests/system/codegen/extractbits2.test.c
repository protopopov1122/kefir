#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long extractbits(long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    ASSERT(extractbits(0x89abcdef) == -1 + -2 + -3 + -4 + -5 + -6 + -7 + -8);
    for (int i = 0; i <= 0xf; i++) {
        int ix = i < 8 ? i : (~0xf | i);
        for (int j = 0; j <= 0xf; j++) {
            int jx = j < 8 ? j : (~0xf | j);
            for (int k = 0; k <= 0xf; k++) {
                int kx = k < 8 ? k : (~0xf | k);
                for (int a = 0; a <= 0xf; a++) {
                    int ax = a < 8 ? a : (~0xf | a);
                    ASSERT(extractbits((i << 12) | (j << 8) | (k << 4) | a) == ix + jx + kx + ax);
                }
            }
        }    
    }
    return EXIT_SUCCESS;
}
