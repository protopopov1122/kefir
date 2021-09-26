#define INTERNAL
#include "./definitions.h"

void wide_greet(struct wide_string *wstr) {
    *wstr = (struct wide_string){L'H', L'e', L'l', L'l', L'o', L',', L' ', L'w', L'o', L'r', L'l', L'd', L'!', L'\0'};
}