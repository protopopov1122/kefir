#ifdef INTERNAL
typedef unsigned short char16_t;
typedef unsigned int char32_t;
typedef int wchar_t;
#endif

struct content {
    const char16_t *literal1;
    char16_t literal2[32];
    const char32_t *literal3;
    char32_t literal4[32];
    const wchar_t *literal5;
    wchar_t literal6[32];
};

struct content init_content();
