#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

size_t custom_strlen(const char[]);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    const char *STRINGS[] = {
        "", "\0", "1", "123", "\0 123", "\t\t\t  ", "\t\t\t  \0", "HELLO, WORLD!", "HELLO, brutal WoRlD!\n\0"};
    size_t STRINGS_LENGTH = sizeof(STRINGS) / sizeof(STRINGS[0]);

    for (size_t i = 0; i < STRINGS_LENGTH; i++) {
        ASSERT(strlen(STRINGS[i]) == custom_strlen(STRINGS[i]));
    }
    return EXIT_SUCCESS;
}
