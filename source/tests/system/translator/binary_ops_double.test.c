#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

double sum(double, double);
double sub(double, double);
double mul(double, double);
double divide(double, double);
int less(double, double);
int lesseq(double, double);
int greater(double, double);
int greatereq(double, double);
int equals(double, double);
int noteq(double, double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double i = -10.0f; i < 10.0f; i += 0.1f) {
        for (double j = -10.0f; j < 10.0f; j += 0.1f) {
            ASSERT(DOUBLE_EQUALS(sum(i, j), i + j, DOUBLE_EPSILON));
            ASSERT(DOUBLE_EQUALS(sub(i, j), i - j, DOUBLE_EPSILON));
            ASSERT(DOUBLE_EQUALS(mul(i, j), i * j, DOUBLE_EPSILON));
            ASSERT(DOUBLE_EQUALS(divide(i, j), i / j, DOUBLE_EPSILON));

            ASSERT(less(i, j) == (i < j));
            ASSERT(lesseq(i, j) == (i <= j));
            ASSERT(greater(i, j) == (i > j));
            ASSERT(greatereq(i, j) == (i >= j));
            ASSERT(equals(i, j) == (i == j));
            ASSERT(noteq(i, j) == (i != j));
        }
    }
    return EXIT_SUCCESS;
}
