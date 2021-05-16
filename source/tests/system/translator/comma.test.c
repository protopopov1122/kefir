#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int callback(void *ptr) {
    int *i = (void *) ptr;
    return (*i)++;
}

int comma1(int (*)(void *), void *);
int comma2(int (*)(void *), void *);
int comma5(int (*)(void *), void *);
int comma10(int (*)(void *), void *);
int comma20(int (*)(void *), void *);
int comma50(int (*)(void *), void *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    int i = 0;
    ASSERT(comma1(callback, &i) == 0);
    ASSERT(i == 1);

    i = 0;
    ASSERT(comma2(callback, &i) == 1);
    ASSERT(i == 2);

    i = 0;
    ASSERT(comma5(callback, &i) == 4);
    ASSERT(i == 5);

    i = 0;
    ASSERT(comma10(callback, &i) == 9);
    ASSERT(i == 10);

    i = 0;
    ASSERT(comma20(callback, &i) == 19);
    ASSERT(i == 20);

    i = 0;
    ASSERT(comma50(callback, &i) == 49);
    ASSERT(i == 50);
    return EXIT_SUCCESS;
}
