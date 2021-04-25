#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

bool trunc1(long);
char extend8(long);
short extend16(long);
int extend32(long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        ASSERT(trunc1(i) || i == 0);
    }
    for (int i = -1000; i < 1000; i++) {
        ASSERT(extend8(i) == ((char) i));
    }
    for (int i = -100000; i < 100000; i++) {
        ASSERT(extend16(i) == ((short) i));
        ASSERT(extend32(i) == i);
        const long val = 1000000l * i;
        ASSERT(extend32(val) == ((int) val));
    }
    return EXIT_SUCCESS;
}