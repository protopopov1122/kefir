#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

const char *string1();
const char *string2();
const char *string3();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(strcmp(string1(), "Hello, world!") == 0);
    ASSERT(strcmp(string2(), "\n\n\t\tHey there\'\"!\v\n") == 0);
    ASSERT(string3()[0] == '\0');
    ASSERT(string3()[1] == '\0');
    ASSERT(string3()[2] == '\0');
    ASSERT(string3()[3] == '\0');
    ASSERT(strcmp(string1(), "Hello, world!") == 0);
    ASSERT(string3()[0] == '\0');
    ASSERT(string3()[1] == '\0');
    ASSERT(string3()[2] == '\0');
    ASSERT(string3()[3] == '\0');
    ASSERT(strcmp(string2(), "\n\n\t\tHey there\'\"!\v\n") == 0);
    return EXIT_SUCCESS;
}