#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

void copybool(const bool *, bool *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    bool b1 = false, b2 = true;

    REQUIRE(b1 == false, KEFIR_INTERNAL_ERROR);
    copybool(&(bool){true}, &b1);
    REQUIRE(b1 == true, KEFIR_INTERNAL_ERROR);

    REQUIRE(b2 == true, KEFIR_INTERNAL_ERROR);
    copybool(&(bool){false}, &b2);
    REQUIRE(b2 == false, KEFIR_INTERNAL_ERROR);
    return EXIT_SUCCESS;
}