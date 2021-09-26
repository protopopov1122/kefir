#define INTERNAL
#include "./definitions.h"

void char32_greet(struct char32_string *wstr) {
    *wstr = (struct char32_string){U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!', U'\0'};
}