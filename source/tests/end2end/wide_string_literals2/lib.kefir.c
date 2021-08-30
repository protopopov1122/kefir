typedef unsigned short char16_t;
typedef unsigned int char32_t;
typedef int wchar_t;

struct content {
    const char16_t *literal1;
    char16_t literal2[32];
    const char32_t *literal3;
    char32_t literal4[32];
    const wchar_t *literal5;
    wchar_t literal6[32];
};

struct content init_content() {
    return (struct content){.literal1 = u"String literal"
                                        "#1"
                                        u".",
                            .literal2 = "S"
                                        "t"
                                        "r"
                                        "i"
                                        "ng"
                                        u" literal#"
                                        "2.",
                            .literal3 = "Stri"
                                        "ng literal#3"
                                        U".",
                            .literal4 = U"Strin"
                                        "g"
                                        " "
                                        "l"
                                        "i"
                                        U"ter"
                                        "al#"
                                        U"4.",
                            .literal5 = L"Str"
                                        "ing lite"
                                        L"ral#"
                                        "5"
                                        ".",
                            .literal6 = "String lit"
                                        L"eral#"
                                        L"6."};
}