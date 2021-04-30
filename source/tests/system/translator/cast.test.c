#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int char_int_cast(char);
char int_char_cast(int);
int uchar_int_cast(unsigned char);
unsigned char int_uchar_cast(int);
unsigned int char_uint_cast(char);
char uint_char_cast(unsigned int);
unsigned int uchar_uint_cast(unsigned char);
unsigned char uint_uchar_cast(unsigned int);
float long_float_cast(long);
long float_long_cast(float);

static void integral_casts() {
    for (char c = CHAR_MIN; c < CHAR_MAX; c++) {
        ASSERT(char_int_cast(c) == ((int) c));
        ASSERT(char_uint_cast(c) == ((unsigned int) c));
    }

    for (unsigned char c = 0; c < UCHAR_MAX; c++) {
        ASSERT(uchar_int_cast(c) == ((int) c));
        ASSERT(uchar_uint_cast(c) == ((unsigned int) c));
    }

    for (int i = -1000; i < 1000; i++) {
        ASSERT(int_char_cast(i) == ((char) i));
        ASSERT(int_uchar_cast(i) == ((unsigned char) i));
    }

    for (unsigned int i = 0; i < 3000; i++) {
        ASSERT(uint_char_cast(i) == ((char) i));
        ASSERT(uint_uchar_cast(i) == ((unsigned char) i));
    }
}

static void floating_point_casts() {
    for (long i = -0x7ff; i < 0x7ff; i++) {
        ASSERT(FLOAT_EQUALS(long_float_cast(i), ((float) i), FLOAT_EPSILON));
    }

    for (float f = -100.0f; f < 100.0f; f += 0.5f) {
        ASSERT(float_long_cast(f) == ((long) f));
    }
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    integral_casts();
    floating_point_casts();
    return EXIT_SUCCESS;
}