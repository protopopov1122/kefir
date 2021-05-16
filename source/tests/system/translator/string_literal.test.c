#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

const char *literal1(int);
const char *literal2(int);
const char *literal3(int);
const char *literal4(int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    const char *STRINGS[] = {
        "Test123, one two three...",
        "Hello, world!",
        "\t\t\n\t\tHell \vwrld!\n\t\0"
    };
    for (unsigned int i = 0; i < strlen(STRINGS[0]); i++) {
        ASSERT(strcmp(STRINGS[0] + i, literal1(i)) == 0);
    }
    for (unsigned int i = 0; i < strlen(STRINGS[1]); i++) {
        ASSERT(strcmp(STRINGS[1] + i, literal2(i)) == 0);
    }
    ASSERT(*literal3(0) == '\0');
    for (unsigned int i = 0; i < strlen(STRINGS[2]); i++) {
        ASSERT(strcmp(STRINGS[2] + i, literal4(i)) == 0);
    }
    return EXIT_SUCCESS;
}
