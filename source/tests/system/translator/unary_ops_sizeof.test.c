#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct sizes {
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
    int tstring;
};

struct sizes get_sizeof();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    struct sizes sz = get_sizeof();
    ASSERT(sz.tboolean == sizeof(bool));
    ASSERT(sz.tchar == sizeof(char));
    ASSERT(sz.tuchar == sizeof(unsigned char));
    ASSERT(sz.tschar == sizeof(signed char));
    ASSERT(sz.tushort == sizeof(unsigned short));
    ASSERT(sz.tsshort == sizeof(signed short));
    ASSERT(sz.tuint == sizeof(unsigned int));
    ASSERT(sz.tsint == sizeof(signed int));
    ASSERT(sz.tulong == sizeof(unsigned long));
    ASSERT(sz.tslong == sizeof(signed long));
    ASSERT(sz.tullong == sizeof(unsigned long long));
    ASSERT(sz.tsllong == sizeof(signed long long));
    ASSERT(sz.tfloat == sizeof(float));
    ASSERT(sz.tdouble == sizeof(double));
    ASSERT(sz.tpvoid == sizeof(void *));
    ASSERT(sz.tstruct == sizeof(struct sizes));
    ASSERT(sz.tarray == sizeof(int[128]));
    ASSERT(sz.tstring == sizeof("Hello, world!!!"));
    return EXIT_SUCCESS;
}
