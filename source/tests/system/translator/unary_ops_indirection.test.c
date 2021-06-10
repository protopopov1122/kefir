#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int get_int(int *, int);
char get_char(char *, int);
double get_double(double *, int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

#define LEN 128
    int INTS[LEN];
    char CHARS[] = "Hello, cruel world!";
    double DOUBLES[LEN];
    for (int i = 0; i < LEN; i++) {
        INTS[i] = ~i;
        DOUBLES[i] = 1.6549e8 * i;
    }

    for (int i = 0; i < LEN; i++) {
        ASSERT(get_int(INTS, i) == ~i);
        ASSERT(DOUBLE_EQUALS(get_double(DOUBLES, i), DOUBLES[i], DOUBLE_EPSILON));
    }
    for (int i = 0; i < (int) sizeof(CHARS); i++) {
        ASSERT(get_char(CHARS, i) == CHARS[i]);
    }
    return EXIT_SUCCESS;
}
