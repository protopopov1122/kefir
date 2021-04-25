#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long insert00(long, long, long);
long insert01(long, long, long);
long insert02(long, long, long);
long insert10(long, long, long);
long insert11(long, long, long);
long insert12(long, long, long);
long insert20(long, long, long);
long insert21(long, long, long);
long insert22(long, long, long);

long xchg00(long, long, long);
long xchg01(long, long, long);
long xchg02(long, long, long);
long xchg10(long, long, long);
long xchg11(long, long, long);
long xchg12(long, long, long);
long xchg20(long, long, long);
long xchg21(long, long, long);
long xchg22(long, long, long);

long drop00(long, long, long);
long drop01(long, long, long);
long drop10(long, long, long);
long drop11(long, long, long);
long drop20(long, long, long);
long drop21(long, long, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(insert00(1, 2, 3) == 3);
    ASSERT(insert01(1, 2, 3) == 2);
    ASSERT(insert02(1, 2, 3) == 1);
    ASSERT(insert10(1, 2, 3) == 2);
    ASSERT(insert11(1, 2, 3) == 3);
    ASSERT(insert12(1, 2, 3) == 1);
    ASSERT(insert20(1, 2, 3) == 1);
    ASSERT(insert21(1, 2, 3) == 3);
    ASSERT(insert22(1, 2, 3) == 2);

    ASSERT(xchg00(1, 2, 3) == 3);
    ASSERT(xchg01(1, 2, 3) == 2);
    ASSERT(xchg02(1, 2, 3) == 1);
    ASSERT(xchg10(1, 2, 3) == 2);
    ASSERT(xchg11(1, 2, 3) == 3);
    ASSERT(xchg12(1, 2, 3) == 1);
    ASSERT(xchg20(1, 2, 3) == 1);
    ASSERT(xchg21(1, 2, 3) == 2);
    ASSERT(xchg22(1, 2, 3) == 3);

    ASSERT(drop00(1, 2, 3) == 2);
    ASSERT(drop01(1, 2, 3) == 1);
    ASSERT(drop10(1, 2, 3) == 3);
    ASSERT(drop11(1, 2, 3) == 1);
    ASSERT(drop20(1, 2, 3) == 3);
    ASSERT(drop21(1, 2, 3) == 2);
    return EXIT_SUCCESS;
}