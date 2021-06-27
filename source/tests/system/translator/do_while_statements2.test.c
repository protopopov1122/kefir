#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long count_positive(unsigned long[], unsigned long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(count_positive((unsigned long[]){1, 2, 3, 4, 0, 1, -1, 10}, -1) == 5);
    ASSERT(count_positive((unsigned long[]){-10, 0, 0, 0, 1, 2, 3, 4, 5, 0}, -1) == 0);
    ASSERT(count_positive((unsigned long[]){1, 2, 3, 4, 5}, 3) == 3);
    ASSERT(count_positive((unsigned long[]){1, 2, 3, 4, 5}, 4) == 4);
    ASSERT(count_positive((unsigned long[]){100, 200, 300, 0, 0, 0, 400, 500, -1, 600}, 10) == 5);
    ASSERT(count_positive((unsigned long[]){0, 0, 0, 0, 0, 1}, 5) == 0);
    ASSERT(count_positive((unsigned long[]){1, 0, 0, 0, 0, 1}, 5) == 1);
    return EXIT_SUCCESS;
}
