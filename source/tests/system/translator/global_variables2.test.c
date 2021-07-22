#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

char *get_character1();
unsigned *get_integer1();
long *get_long1();
float *get_float1();
double *get_double1();
char *get_str1();
char **get_str2();
char **get_str3();
unsigned **get_int1ptr();
void **get_fnptr();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(*get_character1() == 'C');
    ASSERT(*get_integer1() == 0xfefea6);
    ASSERT(*get_long1() == -1234543);
    ASSERT(FLOAT_EQUALS(*get_float1(), 9.57463f, FLOAT_EPSILON));
    ASSERT(DOUBLE_EQUALS(*get_double1(), 1.564e14, DOUBLE_EPSILON));
    ASSERT(strcmp(get_str1(), "Ping-pong-ping-pong") == 0);
    ASSERT(strcmp(*get_str2(), "   ....\t\t\t\\\n\n\n...TEST\n ") == 0);
    ASSERT(*get_str3() == get_str1());
    ASSERT(strcmp(*get_str3(), get_str1()) == 0);
    ASSERT(*get_int1ptr() == get_integer1());
    ASSERT(**get_int1ptr() == 0xfefea6);
    unsigned *(*someptr)() = get_integer1;
    ASSERT(*get_fnptr() == *(void **) &someptr);
    return EXIT_SUCCESS;
}
