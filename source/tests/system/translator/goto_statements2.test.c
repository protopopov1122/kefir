#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int indexof(const char[], char);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    const char *STRING = "Hello, world!";
    for (unsigned int i = 0; i < strlen(STRING); i++) {
        const char *ptr = strchr(STRING, STRING[i]);
        ASSERT(indexof(STRING, STRING[i]) == ptr - STRING);
    }
    ASSERT(indexof("Hello, world!", 'h') == -1);
    ASSERT(indexof("Hello, world!", '\t') == -1);
    ASSERT(indexof("Hello, world!", '\0') == -1);
    return EXIT_SUCCESS;
}
