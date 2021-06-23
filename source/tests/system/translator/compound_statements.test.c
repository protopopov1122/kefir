#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

double compound_test(double, double, double, double, double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double base = -2.0; base < 2.0; base += 0.1) {
        for (double add = -4.0; add < 4.0; add += 0.1) {
            for (double multiply = -100; multiply < 100; multiply += 25) {
                ASSERT(DOUBLE_EQUALS((base + add) / (1 / base) * multiply - add,
                                     compound_test(base, add, 1 / base, multiply, add), DOUBLE_EPSILON));
            }
        }
    }
    return EXIT_SUCCESS;
}
