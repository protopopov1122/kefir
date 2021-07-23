#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

void fn1() {}

extern char character1;
extern unsigned integer1;
extern long long1;
extern float float1;
extern double double1;
extern char str1[];
extern char *str2;
extern char *str3;
extern unsigned *int1ptr;
extern void *fn1ptr;
extern void *null_ptr;

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(character1 == 'B');
    ASSERT(integer1 == 0x4efd);
    ASSERT(long1 == -100000);
    ASSERT(FLOAT_EQUALS(float1, 7.6549f, FLOAT_EPSILON));
    ASSERT(DOUBLE_EQUALS(double1, 0.547e-9, DOUBLE_EPSILON));
    ASSERT(strcmp(str1, "Test...test...test....") == 0);
    ASSERT(strcmp(str2, "TEXT-ONE-TWO-THREE") == 0);
    ASSERT(str3 == str1);
    ASSERT(strcmp(str3, str1) == 0);
    ASSERT(int1ptr == &integer1);
    ASSERT(*int1ptr == 0x4efd);
    void (*someptr)() = &fn1;
    ASSERT(fn1ptr == *(void **) &someptr);
    ASSERT(null_ptr == NULL);
    return EXIT_SUCCESS;
}
