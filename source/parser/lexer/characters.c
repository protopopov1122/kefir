#include "kefir/parser/lexer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/util/char32.h"
#include "kefir/core/source_error.h"

static kefir_uint32_t oct_to_digit(kefir_char32_t chr) {
    if (chr >= U'0' && chr <= U'7') {
        return chr - U'0';
    } else {
        return ~((kefir_uint32_t) 0);
    }
}

kefir_result_t kefir_lexer_cursor_next_universal_character(struct kefir_lexer_source_cursor *cursor,
                                                           kefir_char32_t *target) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer source cursor"));
    REQUIRE(target != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to character"));

    kefir_char32_t chr = kefir_lexer_source_cursor_at(cursor, 0);
    kefir_char32_t chr2 = kefir_lexer_source_cursor_at(cursor, 1);

    if (chr == U'\\' && chr2 == U'u') {
        kefir_char32_t hex1 = kefir_lexer_source_cursor_at(cursor, 2), hex2 = kefir_lexer_source_cursor_at(cursor, 3),
                       hex3 = kefir_lexer_source_cursor_at(cursor, 4), hex4 = kefir_lexer_source_cursor_at(cursor, 5);
        REQUIRE(kefir_ishexdigit32(hex1) && kefir_ishexdigit32(hex2) && kefir_ishexdigit32(hex3) &&
                    kefir_ishexdigit32(hex4),
                KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &cursor->location, "Incomplete universal character"));
        *target = kefir_hex32todec(hex4) | (kefir_hex32todec(hex3) << 4) | (kefir_hex32todec(hex2) << 8) |
                  (kefir_hex32todec(hex1) << 12);
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 6));
    } else if (chr == U'\\' && chr2 == U'U') {
        kefir_char32_t hex1 = kefir_lexer_source_cursor_at(cursor, 2), hex2 = kefir_lexer_source_cursor_at(cursor, 3),
                       hex3 = kefir_lexer_source_cursor_at(cursor, 4), hex4 = kefir_lexer_source_cursor_at(cursor, 5),
                       hex5 = kefir_lexer_source_cursor_at(cursor, 6), hex6 = kefir_lexer_source_cursor_at(cursor, 7),
                       hex7 = kefir_lexer_source_cursor_at(cursor, 8), hex8 = kefir_lexer_source_cursor_at(cursor, 9);
        REQUIRE(kefir_ishexdigit32(hex1) && kefir_ishexdigit32(hex2) && kefir_ishexdigit32(hex3) &&
                    kefir_ishexdigit32(hex4) && kefir_ishexdigit32(hex5) && kefir_ishexdigit32(hex6) &&
                    kefir_ishexdigit32(hex7) && kefir_ishexdigit32(hex8),
                KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &cursor->location, "Incomplete universal character"));
        *target = kefir_hex32todec(hex8) | (kefir_hex32todec(hex7) << 4) | (kefir_hex32todec(hex6) << 8) |
                  (kefir_hex32todec(hex5) << 12) | (kefir_hex32todec(hex4) << 16) | (kefir_hex32todec(hex3) << 20) |
                  (kefir_hex32todec(hex2) << 24) | (kefir_hex32todec(hex1) << 28);
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 10));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match universal character");
    }
    return KEFIR_OK;
}

static kefir_result_t next_simple_escape_sequence(struct kefir_lexer_source_cursor *cursor, kefir_char32_t *target) {
    switch (kefir_lexer_source_cursor_at(cursor, 1)) {
        case U'\'':
            *target = U'\'';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U'\"':
            *target = U'\"';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U'?':
            *target = U'\?';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U'\\':
            *target = U'\\';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U'a':
            *target = U'\a';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U'b':
            *target = U'\b';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U'f':
            *target = U'\f';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U'n':
            *target = U'\n';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U'r':
            *target = U'\r';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U't':
            *target = U'\t';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        case U'v':
            *target = U'\v';
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match simple escape sequence");
    }
    return KEFIR_OK;
}

static kefir_result_t next_octal_escape_sequence(struct kefir_lexer_source_cursor *cursor, kefir_char32_t *target) {
    kefir_char32_t chr1 = kefir_lexer_source_cursor_at(cursor, 1);
    kefir_char32_t chr2 = kefir_lexer_source_cursor_at(cursor, 2);
    kefir_char32_t chr3 = kefir_lexer_source_cursor_at(cursor, 3);

    if (kefir_isoctdigit32(chr1) && kefir_isoctdigit32(chr2) && kefir_isoctdigit32(chr3)) {
        *target = (oct_to_digit(chr1) << 6) | (oct_to_digit(chr2) << 3) | oct_to_digit(chr3);
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 4));
    } else if (kefir_isoctdigit32(chr1) && kefir_isoctdigit32(chr2)) {
        *target = (oct_to_digit(chr1) << 3) | oct_to_digit(chr2);
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 3));
    } else if (kefir_isoctdigit32(chr1)) {
        *target = oct_to_digit(chr1);
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match octal escape sequence");
    }
    return KEFIR_OK;
}

static kefir_result_t next_hexadecimal_escape_sequence(struct kefir_lexer_source_cursor *cursor,
                                                       kefir_char32_t *target) {
    REQUIRE(kefir_lexer_source_cursor_at(cursor, 1) == U'x',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match hexadecimal escape sequence"));

    REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
    kefir_char32_t chr = kefir_lexer_source_cursor_at(cursor, 0);
    REQUIRE(kefir_ishexdigit32(chr),
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &cursor->location, "Expected hexadecimal digit"));
    *target = 0;
    for (; kefir_ishexdigit32(chr);
         kefir_lexer_source_cursor_next(cursor, 1), chr = kefir_lexer_source_cursor_at(cursor, 0)) {
        *target <<= 4;
        *target += kefir_hex32todec(chr);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_cursor_next_escape_sequence(struct kefir_lexer_source_cursor *cursor,
                                                       kefir_char32_t *target) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer source cursor"));
    REQUIRE(target != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to character"));
    REQUIRE(kefir_lexer_source_cursor_at(cursor, 0) == U'\\',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match escape sequence"));

    kefir_result_t res = next_simple_escape_sequence(cursor, target);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = next_octal_escape_sequence(cursor, target);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = next_hexadecimal_escape_sequence(cursor, target);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    REQUIRE_OK(kefir_lexer_cursor_next_universal_character(cursor, target));
    return KEFIR_OK;
}
