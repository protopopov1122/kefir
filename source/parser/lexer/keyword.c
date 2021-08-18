#include "kefir/parser/lexer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

_Thread_local struct KeywordEntry {
    const char32_t *literal;
    kefir_keyword_token_t keyword;
} KEYWORDS[] = {{U"auto", KEFIR_KEYWORD_AUTO},
                {U"break", KEFIR_KEYWORD_BREAK},
                {U"case", KEFIR_KEYWORD_CASE},
                {U"char", KEFIR_KEYWORD_CHAR},
                {U"const", KEFIR_KEYWORD_CONST},
                {U"continue", KEFIR_KEYWORD_CONTINUE},
                {U"default", KEFIR_KEYWORD_DEFAULT},
                {U"do", KEFIR_KEYWORD_DO},
                {U"double", KEFIR_KEYWORD_DOUBLE},
                {U"else", KEFIR_KEYWORD_ELSE},
                {U"enum", KEFIR_KEYWORD_ENUM},
                {U"extern", KEFIR_KEYWORD_EXTERN},
                {U"float", KEFIR_KEYWORD_FLOAT},
                {U"for", KEFIR_KEYWORD_FOR},
                {U"goto", KEFIR_KEYWORD_GOTO},
                {U"if", KEFIR_KEYWORD_IF},
                {U"inline", KEFIR_KEYWORD_INLINE},
                {U"int", KEFIR_KEYWORD_INT},
                {U"long", KEFIR_KEYWORD_LONG},
                {U"register", KEFIR_KEYWORD_REGISTER},
                {U"restrict", KEFIR_KEYWORD_RESTRICT},
                {U"return", KEFIR_KEYWORD_RETURN},
                {U"short", KEFIR_KEYWORD_SHORT},
                {U"signed", KEFIR_KEYWORD_SIGNED},
                {U"sizeof", KEFIR_KEYWORD_SIZEOF},
                {U"static", KEFIR_KEYWORD_STATIC},
                {U"struct", KEFIR_KEYWORD_STRUCT},
                {U"switch", KEFIR_KEYWORD_SWITCH},
                {U"typedef", KEFIR_KEYWORD_TYPEDEF},
                {U"union", KEFIR_KEYWORD_UNION},
                {U"unsigned", KEFIR_KEYWORD_UNSIGNED},
                {U"void", KEFIR_KEYWORD_VOID},
                {U"volatile", KEFIR_KEYWORD_VOLATILE},
                {U"while", KEFIR_KEYWORD_WHILE},
                {U"_Alignas", KEFIR_KEYWORD_ALIGNAS},
                {U"_Alignof", KEFIR_KEYWORD_ALIGNOF},
                {U"_Atomic", KEFIR_KEYWORD_ATOMIC},
                {U"_Bool", KEFIR_KEYWORD_BOOL},
                {U"_Complex", KEFIR_KEYWORD_COMPLEX},
                {U"_Generic", KEFIR_KEYWORD_GENERIC},
                {U"_Imaginary", KEFIR_KEYWORD_IMAGINARY},
                {U"_Noreturn", KEFIR_KEYWORD_NORETURN},
                {U"_Static_assert", KEFIR_KEYWORD_STATIC_ASSERT},
                {U"_Thread_local", KEFIR_KEYWORD_THREAD_LOCAL}};

static kefir_size_t c32strlen(const char32_t *string) {
    REQUIRE(string != NULL, 0);
    kefir_size_t length = 0;
    while (string[length] != U'\0') {
        length++;
    }
    return length;
}

static int keyword_cmp(const void *k1, const void *k2) {
    ASSIGN_DECL_CAST(struct KeywordEntry *, entry1, k1);
    ASSIGN_DECL_CAST(struct KeywordEntry *, entry2, k2);
    const kefir_size_t length1 = c32strlen(entry1->literal);
    const kefir_size_t length2 = c32strlen(entry2->literal);
    if (length1 > length2) {
        return -1;
    } else if (length1 == length2) {
        return 0;
    } else {
        return 1;
    }
}

static void init_keywords() {
    static _Thread_local kefir_bool_t init_done = false;
    if (!init_done) {
        qsort(KEYWORDS, sizeof(KEYWORDS) / sizeof(KEYWORDS[0]), sizeof(KEYWORDS[0]), keyword_cmp);
        init_done = true;
    }
}

static kefir_result_t match_keyword_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);
    kefir_bool_t found = false;
    const kefir_size_t KEYWORDS_LENGTH = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);
    for (kefir_size_t i = 0; !found && i < KEYWORDS_LENGTH; i++) {
        struct KeywordEntry *entry = &KEYWORDS[i];
        kefir_result_t res = kefir_lexer_cursor_match_string(lexer->cursor, entry->literal);
        if (res == KEFIR_OK) {
            REQUIRE_OK(kefir_token_new_keyword(entry->keyword, token));
            REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, c32strlen(entry->literal)));
            found = true;
        } else {
            REQUIRE(res == KEFIR_NO_MATCH, res);
        }
    }

    REQUIRE(found, KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match keyword token"));
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_match_keyword(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));

    init_keywords();
    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_keyword_impl, token));
    return KEFIR_OK;
}
