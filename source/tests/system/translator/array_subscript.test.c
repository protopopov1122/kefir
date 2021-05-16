#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct structure {
    char array[32];
};

void set_int(int[], unsigned int, int);
int get_int(int[], unsigned int);
void set_double(double[], unsigned int, double);
double get_double(double[], unsigned int);
void set_struct(struct structure[], unsigned int, struct structure);
struct structure get_struct(struct structure[], unsigned int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

#define LEN 128
    int INTS[LEN];
    double DOUBLES[LEN];
    struct structure STRUCTS[LEN];

    for (unsigned int i = 0; i < LEN; i++) {
        set_int(INTS, i, ~i);
        set_double(DOUBLES, i, 4.12 + i - 1e5 * i);
        set_struct(STRUCTS, i, (struct structure){
            .array = {
                i, i + 1, i + 2, 'H', 'e', 'y', '\0'
            }
        });
    }

    for (unsigned int i = 0; i < LEN; i++) {
        ASSERT(INTS[i] == (int) ~i);
        ASSERT(get_int(INTS, i) == (int) ~i);
        ASSERT(DOUBLE_EQUALS(DOUBLES[i], 4.12 + i - 1e5 * i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(get_double(DOUBLES, i), 4.12 + i - 1e5 * i, DOUBLE_EPSILON));
        struct structure s = get_struct(STRUCTS, i);
        ASSERT(s.array[0] == (char) i);
        ASSERT(s.array[1] == (char) (i + 1));
        ASSERT(s.array[2] == (char) (i + 2));
        ASSERT(strcmp(&s.array[3], "Hey") == 0);
    }
#undef LEN
    return EXIT_SUCCESS;
}
