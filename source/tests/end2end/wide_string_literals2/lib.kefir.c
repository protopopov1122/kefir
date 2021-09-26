#define INTERNAL
#include "definitions.h"

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