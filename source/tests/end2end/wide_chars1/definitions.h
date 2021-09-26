#ifdef INTERNAL
typedef unsigned short char16_t;
typedef unsigned int char32_t;
typedef int wchar_t;
#endif

void wstring_uppercase(wchar_t *);
void u16string_uppercase(char16_t *);
void u32string_uppercase(char32_t *);
