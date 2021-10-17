#include "kefir/preprocessor/preprocessor.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"
#include "kefir/core/string_buffer.h"
#include <string.h>

kefir_result_t kefir_preprocessor_token_convert(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                struct kefir_token *dst, const struct kefir_token *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to destination token"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source token"));

    switch (src->klass) {
        case KEFIR_TOKEN_PP_NUMBER: {
            struct kefir_lexer_source_cursor_state state;
            struct kefir_lexer_source_cursor cursor;
            REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, src->identifier, strlen(src->identifier), ""));
            cursor.location = src->source_location;
            REQUIRE_OK(kefir_lexer_source_cursor_save(&cursor, &state));

            kefir_result_t res = kefir_lexer_scan_floating_point_constant(mem, &cursor, dst);
            if (res == KEFIR_NO_MATCH) {
                REQUIRE_OK(kefir_lexer_source_cursor_restore(&cursor, &state));
                res = kefir_lexer_scan_integral_constant(&cursor, preprocessor->lexer.context, dst);
            }
            REQUIRE_OK(res);
            REQUIRE_ELSE(cursor.index == cursor.length, {
                kefir_token_free(mem, dst);
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &cursor.location,
                                              "Unexpected character in numeric token");
            });
            dst->source_location = src->source_location;
        } break;

        case KEFIR_TOKEN_IDENTIFIER: {
            struct kefir_lexer_source_cursor cursor;
            REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, src->identifier, strlen(src->identifier), ""));
            cursor.location = src->source_location;

            REQUIRE_OK(kefir_lexer_scan_identifier_or_keyword(mem, &cursor, preprocessor->lexer.symbols,
                                                              &preprocessor->lexer.keywords, dst));
            REQUIRE_ELSE(cursor.index == cursor.length, {
                kefir_token_free(mem, dst);
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &cursor.location,
                                              "Unexpected character in identifier/keyword token");
            });
            dst->source_location = src->source_location;
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

static kefir_result_t init_string_buffer(struct kefir_token *literal, struct kefir_string_buffer *strbuf) {
    switch (literal->string_literal.type) {
        case KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE:
            REQUIRE_OK(kefir_string_buffer_init_value(strbuf, KEFIR_STRING_BUFFER_MULTIBYTE,
                                                      literal->string_literal.literal, literal->string_literal.length));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE8:
            REQUIRE_OK(kefir_string_buffer_init_value(strbuf, KEFIR_STRING_BUFFER_UNICODE8,
                                                      literal->string_literal.literal, literal->string_literal.length));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE16:
            REQUIRE_OK(kefir_string_buffer_init_value(strbuf, KEFIR_STRING_BUFFER_UNICODE16,
                                                      literal->string_literal.literal, literal->string_literal.length));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE32:
            REQUIRE_OK(kefir_string_buffer_init_value(strbuf, KEFIR_STRING_BUFFER_UNICODE32,
                                                      literal->string_literal.literal, literal->string_literal.length));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_WIDE:
            REQUIRE_OK(kefir_string_buffer_init_value(strbuf, KEFIR_STRING_BUFFER_WIDE, literal->string_literal.literal,
                                                      literal->string_literal.length));
            break;
    }
    literal->string_literal.literal = NULL;
    literal->string_literal.length = 0;
    return KEFIR_OK;
}

static kefir_result_t join_string_literals_impl(struct kefir_mem *mem, struct kefir_token *literal1,
                                                struct kefir_token *literal2, struct kefir_token *result) {
    struct kefir_string_buffer strbuf1, strbuf2;
    REQUIRE_OK(init_string_buffer(literal1, &strbuf1));
    kefir_result_t res = init_string_buffer(literal2, &strbuf2);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_string_buffer_free(mem, &strbuf1);
        return res;
    });

    if (strbuf1.mode == KEFIR_STRING_BUFFER_MULTIBYTE && strbuf1.mode != strbuf2.mode) {
        res = kefir_string_buffer_convert(mem, &strbuf1, strbuf2.mode);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_string_buffer_free(mem, &strbuf1);
            kefir_string_buffer_free(mem, &strbuf2);
            return res;
        });
    }

    res = kefir_string_buffer_merge(mem, &strbuf1, &strbuf2);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_string_buffer_free(mem, &strbuf1);
        kefir_string_buffer_free(mem, &strbuf2);
        return res;
    });

    res = kefir_string_buffer_free(mem, &strbuf2);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_string_buffer_free(mem, &strbuf1);
        return res;
    });

    kefir_size_t result_length;
    const void *result_value = kefir_string_buffer_value(&strbuf1, &result_length);
    switch (strbuf1.mode) {
        case KEFIR_STRING_BUFFER_MULTIBYTE:
            res = kefir_token_new_string_literal_multibyte(mem, result_value, result_length, result);
            break;

        case KEFIR_STRING_BUFFER_UNICODE8:
            res = kefir_token_new_string_literal_unicode8(mem, result_value, result_length, result);
            break;

        case KEFIR_STRING_BUFFER_UNICODE16:
            res = kefir_token_new_string_literal_unicode16(mem, result_value, result_length, result);
            break;

        case KEFIR_STRING_BUFFER_UNICODE32:
            res = kefir_token_new_string_literal_unicode32(mem, result_value, result_length, result);
            break;

        case KEFIR_STRING_BUFFER_WIDE:
            res = kefir_token_new_string_literal_wide(mem, result_value, result_length, result);
            break;
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_string_buffer_free(mem, &strbuf1);
        return res;
    });

    res = kefir_string_buffer_free(mem, &strbuf1);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, result);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t append_string_literal(struct kefir_mem *mem, struct kefir_token_buffer *dst,
                                            struct kefir_token *literal2) {
    UNUSED(mem);
    if (dst->length == 0 || dst->tokens[dst->length - 1].klass != KEFIR_TOKEN_STRING_LITERAL) {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to join string literals");
    }
    struct kefir_token *literal1 = &dst->tokens[dst->length - 1];
    switch (literal1->string_literal.type) {
        case KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE:
            // Intentionally left blank
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE8:
            REQUIRE(literal2->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE ||
                        literal2->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_UNICODE8,
                    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Can only join unicode8 with other unicode8 or multibyte"));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE16:
            REQUIRE(literal2->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE ||
                        literal2->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_UNICODE16,
                    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Can only join unicode16 with other unicode16 or multibyte"));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE32:
            REQUIRE(literal2->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE ||
                        literal2->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_UNICODE32,
                    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Can only join unicode32 with other unicode32 or multibyte"));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_WIDE:
            REQUIRE(literal2->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE ||
                        literal2->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_WIDE,
                    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Can only join wide with other wide or multibyte"));
            break;
    }

    struct kefir_source_location source_location = literal1->source_location;
    struct kefir_token result;
    REQUIRE_OK(join_string_literals_impl(mem, literal1, literal2, &result));
    result.source_location = source_location;
    kefir_result_t res = kefir_token_buffer_pop(mem, dst);
    REQUIRE_CHAIN(&res, kefir_token_buffer_emplace(mem, dst, &result));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &result);
        return res;
    });
    REQUIRE_OK(kefir_token_free(mem, literal2));
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
        switch (src->tokens[i].klass) {
            case KEFIR_TOKEN_PP_NUMBER:
            case KEFIR_TOKEN_IDENTIFIER:
            case KEFIR_TOKEN_SENTINEL:
            case KEFIR_TOKEN_KEYWORD:
            case KEFIR_TOKEN_CONSTANT:
            case KEFIR_TOKEN_PUNCTUATOR: {
                struct kefir_token token;
                REQUIRE_OK(kefir_preprocessor_token_convert(mem, preprocessor, &token, &src->tokens[i]));
                kefir_result_t res = kefir_token_buffer_emplace(mem, dst, &token);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    kefir_token_free(mem, &token);
                    return res;
                });
            } break;

            case KEFIR_TOKEN_STRING_LITERAL: {
                kefir_result_t res = append_string_literal(mem, dst, &src->tokens[i]);
                if (res == KEFIR_NO_MATCH) {
                    struct kefir_token token;
                    REQUIRE_OK(kefir_preprocessor_token_convert(mem, preprocessor, &token, &src->tokens[i]));
                    kefir_result_t res = kefir_token_buffer_emplace(mem, dst, &token);
                    REQUIRE_ELSE(res == KEFIR_OK, {
                        kefir_token_free(mem, &token);
                        return res;
                    });
                } else {
                    REQUIRE_OK(res);
                }
            } break;

            case KEFIR_TOKEN_PP_WHITESPACE:
                // Skip whitespaces
                break;

            case KEFIR_TOKEN_PP_HEADER_NAME:
            case KEFIR_TOKEN_PP_PLACEMAKER:
                return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                                       "Encountered unexpected preprocessor token during conversion");
        }
    }
    return KEFIR_OK;
}
