#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a : 8;
    short b : 9;
    int c : 6;
    char d : 1;
    char e : 1;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -100; i <= 100; i++) {
        ASSERT(sum(&(struct param){
            .a = 54,
            .b = i,
            .c = -5,
            .d = 0,
            .e = -1
        }) == (int) (54 + i - 5 - 1 + sizeof(struct param) + _Alignof(struct param)));

        ASSERT(sum(&(struct param){
            .a = 54,
            .b = i,
            .c = -5,
            .d = 0,
            .e = 0
        }) == (int) (54 + i - 5 + sizeof(struct param) + _Alignof(struct param)));
        
        ASSERT(sum(&(struct param){
            .a = 54,
            .b = i,
            .c = -5,
            .d = -1,
            .e = 0
        }) == (int) (54 + i - 5 - 1 + sizeof(struct param) + _Alignof(struct param)));

        ASSERT(sum(&(struct param){
            .a = 54,
            .b = i,
            .c = -5,
            .d = -1,
            .e = -1
        }) == (int) (54 + i - 5 - 1 - 1 + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
