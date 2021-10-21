/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/core/platform.h"
#ifdef KEFIR_LINUX_HOST_PLATFORM
#define _POSIX_C_SOURCE 200809L
#endif

#include "kefir/preprocessor/format.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/os_error.h"
#include <ctype.h>
#include <wctype.h>
#include <string.h>
#include <stdio.h>

static kefir_result_t format_punctuator(FILE *out, kefir_punctuator_token_t punctuator) {
    static const char *const PUNCTUATORS[] = {[KEFIR_PUNCTUATOR_LEFT_BRACKET] = "[",
                                              [KEFIR_PUNCTUATOR_RIGHT_BRACKET] = "]",
                                              [KEFIR_PUNCTUATOR_LEFT_PARENTHESE] = "(",
                                              [KEFIR_PUNCTUATOR_RIGHT_PARENTHESE] = ")",
                                              [KEFIR_PUNCTUATOR_LEFT_BRACE] = "{",
                                              [KEFIR_PUNCTUATOR_RIGHT_BRACE] = "}",
                                              [KEFIR_PUNCTUATOR_DOT] = ".",
                                              [KEFIR_PUNCTUATOR_RIGHT_ARROW] = "->",
                                              [KEFIR_PUNCTUATOR_DOUBLE_PLUS] = "++",
                                              [KEFIR_PUNCTUATOR_DOUBLE_MINUS] = "--",
                                              [KEFIR_PUNCTUATOR_AMPERSAND] = "&",
                                              [KEFIR_PUNCTUATOR_STAR] = "*",
                                              [KEFIR_PUNCTUATOR_PLUS] = "+",
                                              [KEFIR_PUNCTUATOR_MINUS] = "-",
                                              [KEFIR_PUNCTUATOR_TILDE] = "~",
                                              [KEFIR_PUNCTUATOR_EXCLAMATION_MARK] = "!",
                                              [KEFIR_PUNCTUATOR_SLASH] = "/",
                                              [KEFIR_PUNCTUATOR_PERCENT] = "%",
                                              [KEFIR_PUNCTUATOR_LEFT_SHIFT] = "<<",
                                              [KEFIR_PUNCTUATOR_RIGHT_SHIFT] = ">>",
                                              [KEFIR_PUNCTUATOR_LESS_THAN] = "<",
                                              [KEFIR_PUNCTUATOR_GREATER_THAN] = ">",
                                              [KEFIR_PUNCTUATOR_LESS_OR_EQUAL] = "<=",
                                              [KEFIR_PUNCTUATOR_GREATER_OR_EQUAL] = ">=",
                                              [KEFIR_PUNCTUATOR_EQUAL] = "==",
                                              [KEFIR_PUNCTUATOR_NOT_EQUAL] = "!=",
                                              [KEFIR_PUNCTUATOR_CARET] = "^",
                                              [KEFIR_PUNCTUATOR_VBAR] = "|",
                                              [KEFIR_PUNCTUATOR_DOUBLE_AMPERSAND] = "&&",
                                              [KEFIR_PUNCTUATOR_DOUBLE_VBAR] = "||",
                                              [KEFIR_PUNCTUATOR_QUESTION_MARK] = "?",
                                              [KEFIR_PUNCTUATOR_COLON] = ":",
                                              [KEFIR_PUNCTUATOR_SEMICOLON] = ";",
                                              [KEFIR_PUNCTUATOR_ELLIPSIS] = "...",
                                              [KEFIR_PUNCTUATOR_ASSIGN] = "=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_MULTIPLY] = "*=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_DIVIDE] = "/=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_MODULO] = "%=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_ADD] = "+=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_SUBTRACT] = "-=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_SHIFT_LEFT] = "<<=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_SHIFT_RIGHT] = ">>=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_AND] = "&=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_XOR] = "^=",
                                              [KEFIR_PUNCTUATOR_ASSIGN_OR] = "|=",
                                              [KEFIR_PUNCTUATOR_COMMA] = ",",
                                              [KEFIR_PUNCTUATOR_HASH] = "#",
                                              [KEFIR_PUNCTUATOR_DOUBLE_HASH] = "##",
                                              [KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACKET] = "[",
                                              [KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACKET] = "]",
                                              [KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACE] = "{",
                                              [KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACE] = "}",
                                              [KEFIR_PUNCTUATOR_DIGRAPH_HASH] = "#",
                                              [KEFIR_PUNCTUATOR_DIGRAPH_DOUBLE_HASH] = "##"};
    fprintf(out, "%s", PUNCTUATORS[punctuator]);
    return KEFIR_OK;
}

static kefir_result_t format_char(FILE *out, char chr) {
    switch (chr) {
        case '\'':
            fprintf(out, "\\\'");
            break;

        case '\"':
            fprintf(out, "\\\"");
            break;

        case '\?':
            fprintf(out, "\\\?");
            break;

        case '\\':
            fprintf(out, "\\\\");
            break;

        case '\a':
            fprintf(out, "\\a");
            break;

        case '\b':
            fprintf(out, "\\b");
            break;

        case '\f':
            fprintf(out, "\\f");
            break;

        case '\n':
            fprintf(out, "\\n");
            break;

        case '\r':
            fprintf(out, "\\r");
            break;

        case '\t':
            fprintf(out, "\\t");
            break;

        case '\v':
            fprintf(out, "\\v");
            break;

        case '\0':
            fprintf(out, "\\0");
            break;

        default:
            if (isprint(chr)) {
                fprintf(out, "%c", chr);
            } else {
                fprintf(out, "\\%03o", chr);
            }
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t format_wchar(FILE *out, wchar_t chr) {
    switch (chr) {
        case L'\'':
            fprintf(out, "\\\'");
            break;

        case L'\"':
            fprintf(out, "\\\"");
            break;

        case L'\?':
            fprintf(out, "\\\?");
            break;

        case L'\\':
            fprintf(out, "\\\\");
            break;

        case L'\a':
            fprintf(out, "\\a");
            break;

        case L'\b':
            fprintf(out, "\\b");
            break;

        case L'\f':
            fprintf(out, "\\f");
            break;

        case L'\n':
            fprintf(out, "\\n");
            break;

        case L'\r':
            fprintf(out, "\\r");
            break;

        case L'\t':
            fprintf(out, "\\t");
            break;

        case L'\v':
            fprintf(out, "\\v");
            break;

        case L'\0':
            fprintf(out, "\\0");
            break;

        default:
            if (iswprint(chr)) {
                fprintf(out, "%lc", chr);
            } else {
                fprintf(out, "\\x%x", chr);
            }
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t format_constant(FILE *out, const struct kefir_token *token) {
    switch (token->constant.type) {
        case KEFIR_CONSTANT_TOKEN_INTEGER:
        case KEFIR_CONSTANT_TOKEN_LONG_INTEGER:
        case KEFIR_CONSTANT_TOKEN_LONG_LONG_INTEGER:
        case KEFIR_CONSTANT_TOKEN_UNSIGNED_INTEGER:
        case KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_INTEGER:
        case KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_LONG_INTEGER:
        case KEFIR_CONSTANT_TOKEN_FLOAT:
        case KEFIR_CONSTANT_TOKEN_DOUBLE:
            return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unexpected constant as preprocessor token");

        case KEFIR_CONSTANT_TOKEN_CHAR:
            fprintf(out, "'");
            REQUIRE_OK(format_char(out, token->constant.character));
            fprintf(out, "'");
            break;

        case KEFIR_CONSTANT_TOKEN_WIDE_CHAR:
            fprintf(out, "'");
            REQUIRE_OK(format_wchar(out, token->constant.wide_char));
            fprintf(out, "'");
            break;

        case KEFIR_CONSTANT_TOKEN_UNICODE16_CHAR: {
            char buffer[MB_LEN_MAX];
            mbstate_t mbstate;
            int sz = (int) c16rtomb(buffer, token->constant.unicode16_char, &mbstate);
            REQUIRE(sz >= 0,
                    KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Failed to convert Unicode16 character to multibyte"));
            fprintf(out, "%*s", sz, buffer);
        } break;

        case KEFIR_CONSTANT_TOKEN_UNICODE32_CHAR: {
            char buffer[MB_LEN_MAX];
            mbstate_t mbstate;
            int sz = (int) c32rtomb(buffer, token->constant.unicode32_char, &mbstate);
            REQUIRE(sz >= 0,
                    KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Failed to convert Unicode32 character to multibyte"));
            fprintf(out, "%*s", sz, buffer);
        } break;
    }
    return KEFIR_OK;
}

static kefir_result_t format_string_literal(FILE *out, const struct kefir_token *token) {
    switch (token->string_literal.type) {
        case KEFIR_STRING_LITERAL_TOKEN_UNICODE8:
            fprintf(out, "u8");
            // Fallthrough
        case KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE:
            fprintf(out, "\"");
            for (kefir_size_t i = 0; i < token->string_literal.length - 1; i++) {
                char chr = ((const char *) token->string_literal.literal)[i];
                REQUIRE_OK(format_char(out, chr));
            }
            fprintf(out, "\"");
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE16:
            fprintf(out, "u\"");
            for (kefir_size_t i = 0; i < token->string_literal.length - 1; i++) {
                kefir_char16_t chr = ((const kefir_char16_t *) token->string_literal.literal)[i];
                fprintf(out, "\\x%x", chr);
            }
            fprintf(out, "\"");
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE32:
            fprintf(out, "U\"");
            for (kefir_size_t i = 0; i < token->string_literal.length - 1; i++) {
                kefir_char32_t chr = ((const kefir_char32_t *) token->string_literal.literal)[i];
                fprintf(out, "\\x%x", chr);
            }
            fprintf(out, "\"");
            break;

        case KEFIR_STRING_LITERAL_TOKEN_WIDE:
            fprintf(out, "L\"");
            for (kefir_size_t i = 0; i < token->string_literal.length - 1; i++) {
                kefir_wchar_t chr = ((const kefir_wchar_t *) token->string_literal.literal)[i];
                if (iswprint(chr)) {
                    fprintf(out, "%lc", chr);
                } else {
                    fprintf(out, "\\x%x", chr);
                }
            }
            fprintf(out, "\"");
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t format_token(FILE *out, const struct kefir_token *token,
                                   kefir_preprocessor_whitespace_format_t ws_format) {
    switch (token->klass) {
        case KEFIR_TOKEN_SENTINEL:
            // Intentionally left blank
            break;

        case KEFIR_TOKEN_KEYWORD:
            return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unexpected keyword as preprocessor token");

        case KEFIR_TOKEN_IDENTIFIER:
            fprintf(out, "%s", token->identifier);
            break;

        case KEFIR_TOKEN_CONSTANT:
            REQUIRE_OK(format_constant(out, token));
            break;

        case KEFIR_TOKEN_STRING_LITERAL:
            REQUIRE_OK(format_string_literal(out, token));
            break;

        case KEFIR_TOKEN_PUNCTUATOR:
            REQUIRE_OK(format_punctuator(out, token->punctuator));
            break;

        case KEFIR_TOKEN_PP_WHITESPACE:
            if (token->pp_whitespace.newline && ws_format == KEFIR_PREPROCESSOR_WHITESPACE_FORMAT_ORIGINAL) {
                fprintf(out, "\n");
            } else {
                fprintf(out, " ");
            }
            break;

        case KEFIR_TOKEN_PP_NUMBER:
            fprintf(out, "%s", token->pp_number.number_literal);
            break;

        case KEFIR_TOKEN_PP_HEADER_NAME:
            if (token->pp_header_name.system) {
                fprintf(out, "<%s>", token->pp_header_name.header_name);
            } else {
                fprintf(out, "\"%s\"", token->pp_header_name.header_name);
            }
            break;

        case KEFIR_TOKEN_PP_PLACEMAKER:
            break;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_format(FILE *out, const struct kefir_token_buffer *buffer,
                                         kefir_preprocessor_whitespace_format_t ws_format) {
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid FILE"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    for (kefir_size_t i = 0; i < buffer->length; i++) {
        REQUIRE_OK(format_token(out, &buffer->tokens[i], ws_format));
    }
    return KEFIR_OK;
}

static kefir_result_t format_string_impl(struct kefir_mem *mem, char **string_ptr,
                                         const struct kefir_token_buffer *buffer,
                                         kefir_preprocessor_whitespace_format_t ws_format, FILE *fp, size_t *sz) {
    REQUIRE_OK(kefir_preprocessor_format(fp, buffer, ws_format));
    int rc = fclose(fp);
    REQUIRE(rc == 0, KEFIR_SET_OS_ERROR("Failed to close in-memory file"));
    *string_ptr = KEFIR_MALLOC(mem, *sz + 1);
    REQUIRE(*string_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to close in-memory file"));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_format_string(struct kefir_mem *mem, char **string_ptr, kefir_size_t *size_ptr,
                                                const struct kefir_token_buffer *buffer,
                                                kefir_preprocessor_whitespace_format_t ws_format) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(string_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to string"));
    REQUIRE(size_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to size"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    char *ptr = NULL;
    size_t sz = 0;
    FILE *fp = open_memstream(&ptr, &sz);
    REQUIRE(fp != NULL, KEFIR_SET_OS_ERROR("Failed to construct in-memory file"));
    kefir_result_t res = format_string_impl(mem, string_ptr, buffer, ws_format, fp, &sz);
    REQUIRE_ELSE(res == KEFIR_OK, {
        free(ptr);
        return res;
    });
    memcpy(*string_ptr, ptr, sz + 1);
    free(ptr);
    *size_ptr = sz + 1;
    return KEFIR_OK;
}
