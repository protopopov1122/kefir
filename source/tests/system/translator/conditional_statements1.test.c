#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int dummy_factorial(int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        int fact = dummy_factorial(i);
        switch (i) {
            case 1:
                ASSERT(fact == 1);
                break;

            case 2:
                ASSERT(fact == 2);
                break;

            case 3:
                ASSERT(fact == 6);
                break;

            case 4:
                ASSERT(fact == 24);
                break;

            case 5:
                ASSERT(fact == 120);
                break;

            case 6:
                ASSERT(fact == 720);
                break;

            case 7:
                ASSERT(fact == 5040);
                break;

            case 8:
                ASSERT(fact == 40320);
                break;

            case 9:
                ASSERT(fact == 362880);
                break;

            default:
                if (i >= 10) {
                    ASSERT(fact == 3628800);
                } else {
                    ASSERT(fact == -1);
                }
                break;
        }
    }
    return EXIT_SUCCESS;
}
