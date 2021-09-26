#ifdef INTERNAL
typedef unsigned short char16_t;
typedef unsigned int char32_t;
typedef int wchar_t;
#endif

struct wide_string {
    wchar_t content[32];
};

struct char16_string {
    char16_t content[32];
};

struct char32_string {
    char32_t content[32];
};

void wide_greet(struct wide_string *);
void char16_greet(struct char16_string *);
void char32_greet(struct char32_string *);
