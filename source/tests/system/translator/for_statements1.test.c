#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long long sumall(long long[]);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(sumall((long long[]){1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == 45);
    ASSERT(sumall((long long[]){0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == 45);
    ASSERT(sumall((long long[]){1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == 1);
    ASSERT(sumall((long long[]){1000, 100, 900, 200, 0, 300}) == 2200);
    ASSERT(sumall((long long[]){0, 1000, 100, 900, 200, 0, 300}) == 2200);
    ASSERT(sumall((long long[]){0, 0, 1000, 100, 900, 200, 0, 300}) == 0);
    return EXIT_SUCCESS;
}
