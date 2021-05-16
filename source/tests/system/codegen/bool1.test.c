#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

bool bool_and(bool, bool);
bool bool_or(bool, bool);
bool bool_not(bool);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(bool_and(false, false) == false);
    ASSERT(bool_and(false, true) == false);
    ASSERT(bool_and(true, false) == false);
    ASSERT(bool_and(true, true) == true);
    ASSERT(bool_or(false, false) == false);
    ASSERT(bool_or(false, true) == true);
    ASSERT(bool_or(true, false) == true);
    ASSERT(bool_or(true, true) == true);
    ASSERT(bool_not(false) == true);
    ASSERT(bool_not(true) == false);
    return EXIT_SUCCESS;
}
