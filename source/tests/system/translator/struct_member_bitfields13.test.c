#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    unsigned char a : 1;
    unsigned long b : 1;
    unsigned char c : 7;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (unsigned int i = 0; i < 50; i++) {
        ASSERT(sum(&(struct param){
            .a = 0,
            .b = 0,
            .c = i
        }) == (int) (i + sizeof(struct param) + _Alignof(struct param)));

        ASSERT(sum(&(struct param){
            .a = 1,
            .b = 0,
            .c = i
        }) == (int) (i + 1 + sizeof(struct param) + _Alignof(struct param)));

        ASSERT(sum(&(struct param){
            .a = 0,
            .b = 1,
            .c = i
        }) == (int) (i + 1 + sizeof(struct param) + _Alignof(struct param)));

        ASSERT(sum(&(struct param){
            .a = 1,
            .b = 1,
            .c = i
        }) == (int) (i + 2 + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}