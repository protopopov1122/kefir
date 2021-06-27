#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

unsigned long fibonacci(unsigned long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(fibonacci(0) == 0);
    ASSERT(fibonacci(1) == 1);
    ASSERT(fibonacci(2) == 1);
    ASSERT(fibonacci(3) == 2);
    ASSERT(fibonacci(4) == 3);
    ASSERT(fibonacci(5) == 5);
    ASSERT(fibonacci(6) == 8);
    ASSERT(fibonacci(7) == 13);
    ASSERT(fibonacci(8) == 21);
    ASSERT(fibonacci(9) == 34);
    ASSERT(fibonacci(10) == 55);
    ASSERT(fibonacci(11) == 89);
    ASSERT(fibonacci(12) == 144);
    return EXIT_SUCCESS;
}
