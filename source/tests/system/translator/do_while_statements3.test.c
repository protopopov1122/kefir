#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long long sum_matrix(long long *, unsigned int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (long long i = -1000; i < 1000; i++) {
        long long matrix[] = {1 + i, 2, 3, 4, 5, 6 * i, 7, 8, 9 - (~i)};
        ASSERT(sum_matrix(matrix, 3) == 39 + i - (~i) + 6 * i);
    }
    return EXIT_SUCCESS;
}
