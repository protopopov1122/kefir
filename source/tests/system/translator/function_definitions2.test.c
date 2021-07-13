#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int intat(int[], int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

#define BUFSIZE 256
    int BUFFER[BUFSIZE];

    for (int i = 0; i < BUFSIZE; i++) {
        BUFFER[i] = ~i;
    }

    for (int i = 0; i < BUFSIZE; i++) {
        ASSERT(intat(BUFFER, i) == ~i);
    }

    return EXIT_SUCCESS;
}
