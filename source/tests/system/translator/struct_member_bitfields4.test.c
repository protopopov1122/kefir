#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a : 7;
    char b : 7;
    int c : 17;
    char d : 6;
    long e : 40;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    // for (int i = -4; i < 3; i++) {
    //     for (int j = -4; j < 3; j++) {
    //         for (int k = -256; k < 256; k++) {
    //             ASSERT(sum(&(struct param) {
    //                 .f1 = i,
    //                 .f2 = j,
    //                 .f3 = 0,
    //                 .f4 = k << 3
    //             }) == (int) (i + j + (k << 3) + sizeof(struct param) + _Alignof(struct param)));

    //             ASSERT(sum(&(struct param) {
    //                 .f1 = i,
    //                 .f2 = j,
    //                 .f3 = 1,
    //                 .f4 = k << 3
    //             }) == (int) (i + j + (k << 3) + 1 + sizeof(struct param) + _Alignof(struct param)));
    //         }
    //     }
    // }
    ASSERT(sum(&(struct param){
        .a = -64,
        .b = 57,
        .c = 1000,
        .d = -10,
        .e = 0xfffff
    }) == -64 + 57 + 1000 - 10 + 0xfffff + sizeof(struct param) + _Alignof(struct param));
    return EXIT_SUCCESS;
}