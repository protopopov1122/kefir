#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

char int_to_char(int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        char c = int_to_char(i);
        switch (i & 0xf) {
            case 0:
                ASSERT(c == '0');
                break;

            case 1:
                ASSERT(c == '1');
                break;

            case 2:
                ASSERT(c == '2');
                break;

            case 3:
                ASSERT(c == '3');
                break;

            case 4:
                ASSERT(c == '4');
                break;

            case 5:
                ASSERT(c == '5');
                break;

            case 6:
                ASSERT(c == '6');
                break;

            case 7:
                ASSERT(c == '7');
                break;

            case 8:
                ASSERT(c == '8');
                break;

            case 9:
                ASSERT(c == '9');
                break;

            case 10:
                ASSERT(c == 'a');
                break;

            case 11:
                ASSERT(c == 'b');
                break;

            case 12:
                ASSERT(c == 'c');
                break;

            case 13:
                ASSERT(c == 'd');
                break;

            case 14:
                ASSERT(c == 'e');
                break;

            case 15:
                ASSERT(c == 'f');
                break;
        }
    }
    return EXIT_SUCCESS;
}
