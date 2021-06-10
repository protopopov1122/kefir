#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char f1 : 3;
    char f2 : 3;
    unsigned short f3 : 1;
    int f4 : 19;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -4; i < 3; i++) {
        for (int j = -4; j < 3; j++) {
            for (int k = -256; k < 256; k++) {
                int kval = k < 0 ? -(-k << 3) : (k << 3);
                ASSERT(sum(&(struct param){.f1 = i, .f2 = j, .f3 = 0, .f4 = kval}) ==
                       (int) (i + j + kval + sizeof(struct param) + _Alignof(struct param)));

                ASSERT(sum(&(struct param){.f1 = i, .f2 = j, .f3 = 1, .f4 = kval}) ==
                       (int) (i + j + kval + 1 + sizeof(struct param) + _Alignof(struct param)));
            }
        }
    }
    return EXIT_SUCCESS;
}
