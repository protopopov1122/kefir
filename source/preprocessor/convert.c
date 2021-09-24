#include "kefir/preprocessor/preprocessor.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"
#include <string.h>

kefir_result_t kefir_preprocessor_token_convert(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                struct kefir_token *dst, const struct kefir_token *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to destination token"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source token"));

    switch (src->klass) {
        case KEFIR_TOKEN_PP_NUMBER: {
            struct kefir_lexer_source_cursor cursor;
            REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, src->identifier, strlen(src->identifier), NULL));
            cursor.location = src->source_location;

            kefir_result_t res = kefir_lexer_scan_floating_point_constant(mem, &cursor, dst);
            if (res == KEFIR_NO_MATCH) {
                res = kefir_lexer_scan_integral_constant(&cursor, preprocessor->lexer.context, dst);
            }
            REQUIRE_OK(res);
            REQUIRE_ELSE(cursor.index == cursor.length, {
                kefir_token_free(mem, dst);
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &cursor.location,
                                              "Unexpected character in numeric token");
            });
        } break;

        case KEFIR_TOKEN_IDENTIFIER: {
            struct kefir_lexer_source_cursor cursor;
            REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, src->identifier, strlen(src->identifier), NULL));
            cursor.location = src->source_location;

            REQUIRE_OK(kefir_lexer_scan_identifier_or_keyword(mem, &cursor, preprocessor->lexer.symbols,
                                                              &preprocessor->lexer.keywords, dst));
            REQUIRE_ELSE(cursor.index == cursor.length, {
                kefir_token_free(mem, dst);
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &cursor.location,
                                              "Unexpected character in identifier/keyword token");
            });
        } break;

        case KEFIR_TOKEN_SENTINEL:
        case KEFIR_TOKEN_KEYWORD:
        case KEFIR_TOKEN_CONSTANT:
        case KEFIR_TOKEN_STRING_LITERAL:
        case KEFIR_TOKEN_PUNCTUATOR:
            REQUIRE_OK(kefir_token_copy(mem, dst, src));
            break;

        case KEFIR_TOKEN_PP_WHITESPACE:
        case KEFIR_TOKEN_PP_HEADER_NAME:
        case KEFIR_TOKEN_PP_PLACEMAKER:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unable to convert preprocessor token to a lexer token");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_token_convert_buffer(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                       struct kefir_token_buffer *dst,
                                                       const struct kefir_token_buffer *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid destination token buffer"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source token buffer"));

    for (kefir_size_t i = 0; i < src->length; i++) {
        struct kefir_token token;
        REQUIRE_OK(kefir_preprocessor_token_convert(mem, preprocessor, &token, &src->tokens[i]));
        // TODO Join string literals
        kefir_result_t res = kefir_token_buffer_emplace(mem, dst, &token);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_free(mem, &token);
            return res;
        });
    }
    return KEFIR_OK;
}
