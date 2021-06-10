#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct structure {
    const char string[32];
    unsigned int length;
    _Alignas(16) unsigned long padding;
    void *ptr;
    union {
        double fp64;
        float fp32[2];
    } floats[4];
};

struct structure compound1();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    struct structure c1 = compound1();
    ASSERT(strcmp(c1.string, "Goodbye, world!") == 0);
    ASSERT(c1.length == strlen("Goodbye, world!"));
    ASSERT(c1.padding == 1010);
    ASSERT(strcmp(c1.ptr, "Goodbye, world!") == 0);
    ASSERT(DOUBLE_EQUALS(c1.floats[0].fp64, 4.0013, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(c1.floats[1].fp64, 19.88263, DOUBLE_EPSILON));
    ASSERT(FLOAT_EQUALS(c1.floats[2].fp32[0], 1.02f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(c1.floats[2].fp32[1], 2.04f, FLOAT_EPSILON));
    ASSERT(DOUBLE_EQUALS(c1.floats[3].fp64, 1e5, DOUBLE_EPSILON));
    return EXIT_SUCCESS;
}
