#define INTERNAL
#include "./definitions.h"

const char Literal1[] = u8"Test...test..test...\0u8-prefixed string\r\n\0?";
const char16_t Literal2[] = u"x\\\"16-bit\0string\0literal";
const char32_t Literal3[] = U"32-bit string    \t\t\v literal is here\0\0\0";
const wchar_t Literal4[] = L"Wide character\n\r\nis defined as 32-bit char\0\00\000\x0...";

void init_u8str(struct u8string *str) {
    *str = (struct u8string){u8"Hello, world!"};
}

void init_u16str(struct u16string *str) {
    *str = (struct u16string){u"Hello, cruel world!"};
}

void init_u32str(struct u32string *str) {
    *str = (struct u32string){U"Goodbye, cruel world!"};
}

void init_lstr(struct lstring *str) {
    *str = (struct lstring){L"It\'s me, world!"};
}