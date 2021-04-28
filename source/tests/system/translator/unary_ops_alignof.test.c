#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct aligns {
    int tboolean;
    int tchar;
    int tuchar;
    int tschar;
    int tushort;
    int tsshort;
    int tuint;
    int tsint;
    int tulong;
    int tslong;
    int tullong;
    int tsllong;
    int tfloat;
    int tdouble;
    int tpvoid;
    int tstruct;
    int tarray;
};

struct aligns get_alignof();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    struct aligns sz = get_alignof();
    ASSERT(sz.tboolean == _Alignof(bool));
    ASSERT(sz.tchar == _Alignof(char));
    ASSERT(sz.tuchar == _Alignof(unsigned char));
    ASSERT(sz.tschar == _Alignof(signed char));
    ASSERT(sz.tushort == _Alignof(unsigned short));
    ASSERT(sz.tsshort == _Alignof(signed short));
    ASSERT(sz.tuint == _Alignof(unsigned int));
    ASSERT(sz.tsint == _Alignof(signed int));
    ASSERT(sz.tulong == _Alignof(unsigned long));
    ASSERT(sz.tslong == _Alignof(signed long));
    ASSERT(sz.tullong == _Alignof(unsigned long long));
    ASSERT(sz.tsllong == _Alignof(signed long long));
    ASSERT(sz.tfloat == _Alignof(float));
    ASSERT(sz.tdouble == _Alignof(double));
    ASSERT(sz.tpvoid == _Alignof(void *));
    ASSERT(sz.tstruct == _Alignof(struct aligns));
    ASSERT(sz.tarray == _Alignof(int[128]));
    return EXIT_SUCCESS;
}