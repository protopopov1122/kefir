#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct struct_param {
    int length;
    char content[16];
    void *payload;
};

long assign_long(long *, long);
struct struct_param assign_struct(struct struct_param *, struct struct_param);
int multiply_assign_int(int *, int);
float multiply_assign_float(float *, float);
unsigned long divide_assign_ulong(unsigned long *, unsigned long);
double divide_assign_double(double *, double);
short modulo_assign_short(short *, short);
unsigned int shl_assign_uint(unsigned int *, unsigned int);
unsigned int shr_assign_uint(unsigned int *, unsigned int);
long iand_assign_long(long *, long);
long ior_assign_long(long *, long);
long ixor_assign_long(long *, long);
int add_assign_int(int *, int);
double add_assign_double(double *, double);
float *add_assign_floatptr(float **, int);
long long sub_assign_long_long(long long *, long long);
float sub_assign_float(float *, float);
char *sub_assign_charptr(char **, int);

static void simple_assign() {
    long l = 0;
    for (long i = -1000; i < 1000; i++) {
        ASSERT(assign_long(&l, i) == i);
        ASSERT(l == i);
    }

    struct struct_param p = {0};
    struct struct_param p2 = assign_struct(&p, (struct struct_param){
        .length = 5,
        { '1', '2', '3', '4', '5', '\0' },
        &p
    });
    ASSERT(p.length == 5);
    ASSERT(strcmp(p.content, "12345") == 0);
    ASSERT(p.payload == &p);
    ASSERT(p2.length == 5);
    ASSERT(strcmp(p2.content, "12345") == 0);
    ASSERT(p2.payload == &p);
}

static void multiply_assign() {
    for (int i = -250; i < 250; i++) {
        int j = 3;
        ASSERT(multiply_assign_int(&j, i) == 3 * i);
        ASSERT(j == 3 * i);
    }

    for (float f = -10.0f; f < 10.0f; f += 0.1f) {
        float a = 3.14159f;
        float res = multiply_assign_float(&a, f);
        ASSERT(FLOAT_EQUALS(res, 3.14159f * f, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(a, 3.14159f * f, FLOAT_EPSILON));
    }
}

static void divide_assign() {
    for (unsigned long i = 1; i < 2000; i++) {
        unsigned long a = 0xbadcafeu;
        ASSERT(divide_assign_ulong(&a, i) == 0xbadcafeu / i);
        ASSERT(a == 0xbadcafeu / i);
    }
    for (double d = -10.0; d < 10.0; d += 0.1) {
        double a = 2.718281828;
        double res = divide_assign_double(&a, d);
        ASSERT(DOUBLE_EQUALS(res, 2.718281828 / d, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(a, 2.718281828 / d, DOUBLE_EPSILON));
    }
}

static void modulo_assign() {
    for (short i = -0xfff; i < 0xfff; i += 3) {
        if (i != 0) {
            short a = (short) 0xbadb;
            ASSERT(modulo_assign_short(&a, i) == ((short ) 0xbadb) % i);
            ASSERT(a == ((short ) 0xbadb) % i);
        }
    }
}

static void shift_assign() {
    for (unsigned int i = 0; i < 32; i++) {
        unsigned int a = 0xcafebabeu;
        ASSERT(shl_assign_uint(&a, i) == 0xcafebabeu << i);
        ASSERT(a == 0xcafebabeu << i);

        unsigned int b = 0xfeca8ebau;
        ASSERT(shr_assign_uint(&b, i) == 0xfeca8ebau >> i);
        ASSERT(b == 0xfeca8ebau >> i);
    }
}

static void bitwise_assign() {
    for (long l = -2048; l < 2048; l++) {
        long a = 0x6eba1e0c;
        ASSERT(iand_assign_long(&a, l) == (0x6eba1e0c & l));
        ASSERT(a == (0x6eba1e0c & l));

        long b = 0x1fec73ed;
        ASSERT(ior_assign_long(&b, l) == (0x1fec73ed | l));
        ASSERT(b == (0x1fec73ed | l));

        long c = 0x44be902a;
        ASSERT(ixor_assign_long(&c, l) == (0x44be902a ^ l));
        ASSERT(c == (0x44be902a ^ l));
    }
}

static void add_assign() {
    for (int i = -1000; i < 1000; i++) {
        int a = ~i;
        ASSERT(add_assign_int(&a, i) == (~i + i));
        ASSERT(a == (~i + i));
    }

    for (double d = -10.0; d < 10.0; d += 0.1) {
        double a = 5.08142;
        ASSERT(DOUBLE_EQUALS(add_assign_double(&a, d), 5.08142 + d, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(a, 5.08142 + d, DOUBLE_EPSILON));
    }

#define LEN 128
    float FLOATS[LEN];
    for (int i = 0; i < LEN; i++) {
        float *ptr = &FLOATS[0];
        ASSERT(add_assign_floatptr(&ptr, i) == &FLOATS[i]);
        ASSERT(ptr == &FLOATS[i]);
    }
#undef LEN
}

static void sub_assign() {
    for (long long l = -0xffe; l < 0xffe; l++) {
        long long a = -0x6efcba3;
        ASSERT(sub_assign_long_long(&a, l) == (-0x6efcba3 - l));
        ASSERT(a == (-0x6efcba3 - l));
    }

    for (float f = -15.0f; f < 15.0f; f += 0.1f) {
        float a = 105e-5f;
        ASSERT(FLOAT_EQUALS(sub_assign_float(&a, f), (105e-5f - f), FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(a, (105e-5f - f), FLOAT_EPSILON));
    }

#define LEN 256
    char CHARS[LEN];
    for (int i = 0; i < LEN; i++) {
        char *ptr = &CHARS[LEN - 1];
        ASSERT(sub_assign_charptr(&ptr, i) == &CHARS[LEN - 1 - i]);
        ASSERT(ptr == &CHARS[LEN - 1 - i]);
    }
#undef LEN
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    simple_assign();
    multiply_assign();
    divide_assign();
    modulo_assign();
    shift_assign();
    bitwise_assign();
    add_assign();
    sub_assign();
    return EXIT_SUCCESS;
}