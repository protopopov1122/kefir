#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct test {
    signed char array[32];
};

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
double short_double_cast(short);
short double_short_cast(double);
double uint_double_cast(unsigned int);
double float_double_cast(float);
float double_float_cast(double);
void *ulong_voidptr_cast(unsigned long);
unsigned long voidptr_ulong_cast(void *);
void int_void_cast(int);
void double_void_cast(double);
void voidptr_void_cast(void *);
void struct_void_cast(struct test);

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
        ASSERT(DOUBLE_EQUALS(float_double_cast(f), ((double) f), DOUBLE_EPSILON));
    }

    for (short s = -0xfff; s < 0xfff; s++) {
        ASSERT(DOUBLE_EQUALS(short_double_cast(s), ((double) s), DOUBLE_EPSILON));
    }

    for (unsigned int i = 0; i < 0xfe4; i++) {
        ASSERT(DOUBLE_EQUALS(uint_double_cast(i), ((double) i), DOUBLE_EPSILON));
    }

    for (double d = -50.0; d < 50.0; d += 0.25) {
        ASSERT(double_short_cast(d) == ((short) d));
        ASSERT(uint_double_cast(d) == ((unsigned int) d));
        ASSERT(FLOAT_EQUALS(double_float_cast(d), ((float) d), FLOAT_EPSILON));
    }
}

static void pointer_casts() {
    for (unsigned long l = 0; l < 65536; l += 32) {
        ASSERT(ulong_voidptr_cast(l) == ((void *) l));
    }

#define LEN 128
    float FLOATS[LEN];
    for (int i = 0; i < LEN; i++) {
        ASSERT(voidptr_ulong_cast(&FLOATS[i]) == ((unsigned long) &FLOATS[i]));
    }
#undef LEN
}

static void void_casts() {
    for (int i = -100; i < 100; i++) {
        int_void_cast(i);
    }
    for (double d = -15.0; d < 15.0; d += 0.05) {
        double_void_cast(d);
    }
#define LEN 128
    float FLOATS[LEN];
    for (int i = 0; i < LEN; i++) {
        voidptr_void_cast(&FLOATS[i]);
    }
#undef LEN

    struct test tst = {
        "Hello, world!"
    };
    struct_void_cast(tst);
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    integral_casts();
    floating_point_casts();
    pointer_casts();
    void_casts();
    return EXIT_SUCCESS;
}