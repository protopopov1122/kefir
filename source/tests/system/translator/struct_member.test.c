#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param_struct {
    int field1;
    unsigned long field2;
    bool field3;
    float field4;
};

struct param_struct2 {
    struct param_struct *ptr;
    double arg;
};

double sum(struct param_struct2);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -100; i < 100; i++) {
        struct param_struct param1 = {
            .field1 = i * 10,
            .field2 = i * 20,
            .field3 = i & 1,
            .field4 = i * 1e2 + 0.01
        };
        struct param_struct2 param2 = {
            .ptr = &param1,
            .arg = (i * 4) & 0xff
        };

        ASSERT(DOUBLE_EQUALS(sum(param2), param1.field1 + param1.field2 + param1.field3 + param1.field4 + param2.arg,
            DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
