#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct structure {
    int a;
    short b[8];
    double c;
};

void makecopy(const struct structure *, struct structure *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    const struct structure S1 = {
        .a = 100,
        .b = {
            80, 70, 60, 50,
            40, 30, 20, 10
        },
        .c = 3.14159
    };
    struct structure S2 = {0};
    
    makecopy(&S1, &S2);
    REQUIRE(S2.a == 100, KEFIR_INTERNAL_ERROR);
    REQUIRE(S2.b[0] == 80, KEFIR_INTERNAL_ERROR);
    REQUIRE(S2.b[1] == 70, KEFIR_INTERNAL_ERROR);
    REQUIRE(S2.b[2] == 60, KEFIR_INTERNAL_ERROR);
    REQUIRE(S2.b[3] == 50, KEFIR_INTERNAL_ERROR);
    REQUIRE(S2.b[4] == 40, KEFIR_INTERNAL_ERROR);
    REQUIRE(S2.b[5] == 30, KEFIR_INTERNAL_ERROR);
    REQUIRE(S2.b[6] == 20, KEFIR_INTERNAL_ERROR);
    REQUIRE(S2.b[7] == 10, KEFIR_INTERNAL_ERROR);
    REQUIRE(DOUBLE_EQUALS(S2.c, 3.14159, DOUBLE_EPSILON), KEFIR_INTERNAL_ERROR);
    return EXIT_SUCCESS;
}
