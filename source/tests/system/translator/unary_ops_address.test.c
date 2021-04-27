#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

#define LEN 128
int INTS[LEN];
double DOUBLES[LEN];

char CHARS[] = "Hello, world!";

int *int_offset(int);
char *char_offset(int);
double *double_offset(int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = 0; i < LEN; i++) {
        INTS[i] =  i * 2;
        DOUBLES[i] = 5.107 * i;
    }

    for (int i = 0; i < LEN; i++) {
        int *ptr = int_offset(i);
        ASSERT(ptr == &INTS[i]);
        ASSERT(*ptr == i * 2);

        double *ptr2 = double_offset(i);
        ASSERT(ptr2 == &DOUBLES[i]);
        ASSERT(DOUBLE_EQUALS(*ptr2, DOUBLES[i], DOUBLE_EPSILON));
    }
    for (int i = 0; i < (int) sizeof(CHARS); i++) {
        char *ptr = char_offset(i);
        ASSERT(ptr == &CHARS[i]);
        ASSERT(*ptr == CHARS[i]);
    }
    return EXIT_SUCCESS;
}