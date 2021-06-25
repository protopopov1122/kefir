#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long long parseint(const char[]);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(parseint(NULL) == 0);
    ASSERT(parseint("") == 0);
    ASSERT(parseint("-0") == 0);
    ASSERT(parseint("1") == 1);
    ASSERT(parseint("-1") == -1);
    ASSERT(parseint("-1aaaaab") == -1);
    ASSERT(parseint("-.2aaaaab") == 0);
    ASSERT(parseint("80015730") == 80015730);
    ASSERT(parseint("8001 5730") == 8001);
    ASSERT(parseint("-1234") == -1234);
    ASSERT(parseint("10-2") == 10);
    ASSERT(parseint("905648") == 905648);
    ASSERT(parseint("-1112223348168a7") == -1112223348168);
    return EXIT_SUCCESS;
}
