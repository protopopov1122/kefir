#ifdef INTERNAL
typedef unsigned short char16_t;
typedef unsigned int char32_t;
typedef int wchar_t;
#endif

extern const char Literal1[];
extern const char16_t Literal2[];
extern const char32_t Literal3[];
extern const wchar_t Literal4[];

struct u8string {
    char content[24];
};

void init_u8str(struct u8string *);

struct u16string {
    char16_t content[24];
};

void init_u16str(struct u16string *);

struct u32string {
    char32_t content[24];
};

void init_u32str(struct u32string *);

struct lstring {
    wchar_t content[24];
};

void init_lstr(struct lstring *);
