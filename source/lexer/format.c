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

#include "kefir/lexer/format.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t format_keyword(struct kefir_json_output *json, kefir_keyword_token_t keyword) {
    REQUIRE_OK(kefir_json_output_object_key(json, "keyword"));
    switch (keyword) {
        case KEFIR_KEYWORD_AUTO:
            REQUIRE_OK(kefir_json_output_string(json, "auto"));
            break;

        case KEFIR_KEYWORD_BREAK:
            REQUIRE_OK(kefir_json_output_string(json, "break"));
            break;

        case KEFIR_KEYWORD_CASE:
            REQUIRE_OK(kefir_json_output_string(json, "case"));
            break;

        case KEFIR_KEYWORD_CHAR:
            REQUIRE_OK(kefir_json_output_string(json, "char"));
            break;

        case KEFIR_KEYWORD_CONST:
            REQUIRE_OK(kefir_json_output_string(json, "const"));
            break;

        case KEFIR_KEYWORD_CONTINUE:
            REQUIRE_OK(kefir_json_output_string(json, "continue"));
            break;

        case KEFIR_KEYWORD_DEFAULT:
            REQUIRE_OK(kefir_json_output_string(json, "default"));
            break;

        case KEFIR_KEYWORD_DO:
            REQUIRE_OK(kefir_json_output_string(json, "do"));
            break;

        case KEFIR_KEYWORD_DOUBLE:
            REQUIRE_OK(kefir_json_output_string(json, "double"));
            break;

        case KEFIR_KEYWORD_ELSE:
            REQUIRE_OK(kefir_json_output_string(json, "else"));
            break;

        case KEFIR_KEYWORD_ENUM:
            REQUIRE_OK(kefir_json_output_string(json, "enum"));
            break;

        case KEFIR_KEYWORD_EXTERN:
            REQUIRE_OK(kefir_json_output_string(json, "extern"));
            break;

        case KEFIR_KEYWORD_FLOAT:
            REQUIRE_OK(kefir_json_output_string(json, "float"));
            break;

        case KEFIR_KEYWORD_FOR:
            REQUIRE_OK(kefir_json_output_string(json, "for"));
            break;

        case KEFIR_KEYWORD_GOTO:
            REQUIRE_OK(kefir_json_output_string(json, "goto"));
            break;

        case KEFIR_KEYWORD_IF:
            REQUIRE_OK(kefir_json_output_string(json, "if"));
            break;

        case KEFIR_KEYWORD_INLINE:
            REQUIRE_OK(kefir_json_output_string(json, "inline"));
            break;

        case KEFIR_KEYWORD_INT:
            REQUIRE_OK(kefir_json_output_string(json, "int"));
            break;

        case KEFIR_KEYWORD_LONG:
            REQUIRE_OK(kefir_json_output_string(json, "long"));
            break;

        case KEFIR_KEYWORD_REGISTER:
            REQUIRE_OK(kefir_json_output_string(json, "register"));
            break;

        case KEFIR_KEYWORD_RESTRICT:
            REQUIRE_OK(kefir_json_output_string(json, "restrict"));
            break;

        case KEFIR_KEYWORD_RETURN:
            REQUIRE_OK(kefir_json_output_string(json, "return"));
            break;

        case KEFIR_KEYWORD_SHORT:
            REQUIRE_OK(kefir_json_output_string(json, "short"));
            break;

        case KEFIR_KEYWORD_SIGNED:
            REQUIRE_OK(kefir_json_output_string(json, "signed"));
            break;

        case KEFIR_KEYWORD_SIZEOF:
            REQUIRE_OK(kefir_json_output_string(json, "sizeof"));
            break;

        case KEFIR_KEYWORD_STATIC:
            REQUIRE_OK(kefir_json_output_string(json, "static"));
            break;

        case KEFIR_KEYWORD_STRUCT:
            REQUIRE_OK(kefir_json_output_string(json, "struct"));
            break;

        case KEFIR_KEYWORD_SWITCH:
            REQUIRE_OK(kefir_json_output_string(json, "switch"));
            break;

        case KEFIR_KEYWORD_TYPEDEF:
            REQUIRE_OK(kefir_json_output_string(json, "typedef"));
            break;

        case KEFIR_KEYWORD_UNION:
            REQUIRE_OK(kefir_json_output_string(json, "union"));
            break;

        case KEFIR_KEYWORD_UNSIGNED:
            REQUIRE_OK(kefir_json_output_string(json, "unsigned"));
            break;

        case KEFIR_KEYWORD_VOID:
            REQUIRE_OK(kefir_json_output_string(json, "void"));
            break;

        case KEFIR_KEYWORD_VOLATILE:
            REQUIRE_OK(kefir_json_output_string(json, "volatile"));
            break;

        case KEFIR_KEYWORD_WHILE:
            REQUIRE_OK(kefir_json_output_string(json, "while"));
            break;

        case KEFIR_KEYWORD_ALIGNAS:
            REQUIRE_OK(kefir_json_output_string(json, "alignas"));
            break;

        case KEFIR_KEYWORD_ALIGNOF:
            REQUIRE_OK(kefir_json_output_string(json, "alignof"));
            break;

        case KEFIR_KEYWORD_ATOMIC:
            REQUIRE_OK(kefir_json_output_string(json, "atomic"));
            break;

        case KEFIR_KEYWORD_BOOL:
            REQUIRE_OK(kefir_json_output_string(json, "bool"));
            break;

        case KEFIR_KEYWORD_COMPLEX:
            REQUIRE_OK(kefir_json_output_string(json, "complex"));
            break;

        case KEFIR_KEYWORD_GENERIC:
            REQUIRE_OK(kefir_json_output_string(json, "generic"));
            break;

        case KEFIR_KEYWORD_IMAGINARY:
            REQUIRE_OK(kefir_json_output_string(json, "imaginary"));
            break;

        case KEFIR_KEYWORD_NORETURN:
            REQUIRE_OK(kefir_json_output_string(json, "noreturn"));
            break;

        case KEFIR_KEYWORD_STATIC_ASSERT:
            REQUIRE_OK(kefir_json_output_string(json, "static_assert"));
            break;

        case KEFIR_KEYWORD_THREAD_LOCAL:
            REQUIRE_OK(kefir_json_output_string(json, "thread_local"));
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t format_punctuator(struct kefir_json_output *json, kefir_punctuator_token_t punctuator) {
    REQUIRE_OK(kefir_json_output_object_key(json, "punctuator"));
    switch (punctuator) {
        case KEFIR_PUNCTUATOR_LEFT_BRACKET:
            REQUIRE_OK(kefir_json_output_string(json, "["));
            break;

        case KEFIR_PUNCTUATOR_RIGHT_BRACKET:
            REQUIRE_OK(kefir_json_output_string(json, "]"));
            break;

        case KEFIR_PUNCTUATOR_LEFT_PARENTHESE:
            REQUIRE_OK(kefir_json_output_string(json, "("));
            break;

        case KEFIR_PUNCTUATOR_RIGHT_PARENTHESE:
            REQUIRE_OK(kefir_json_output_string(json, ")"));
            break;

        case KEFIR_PUNCTUATOR_LEFT_BRACE:
            REQUIRE_OK(kefir_json_output_string(json, "{"));
            break;

        case KEFIR_PUNCTUATOR_RIGHT_BRACE:
            REQUIRE_OK(kefir_json_output_string(json, "}"));
            break;

        case KEFIR_PUNCTUATOR_DOT:
            REQUIRE_OK(kefir_json_output_string(json, "."));
            break;

        case KEFIR_PUNCTUATOR_RIGHT_ARROW:
            REQUIRE_OK(kefir_json_output_string(json, "->"));
            break;

        case KEFIR_PUNCTUATOR_DOUBLE_PLUS:
            REQUIRE_OK(kefir_json_output_string(json, "++"));
            break;

        case KEFIR_PUNCTUATOR_DOUBLE_MINUS:
            REQUIRE_OK(kefir_json_output_string(json, "--"));
            break;

        case KEFIR_PUNCTUATOR_AMPERSAND:
            REQUIRE_OK(kefir_json_output_string(json, "&"));
            break;

        case KEFIR_PUNCTUATOR_STAR:
            REQUIRE_OK(kefir_json_output_string(json, "*"));
            break;

        case KEFIR_PUNCTUATOR_PLUS:
            REQUIRE_OK(kefir_json_output_string(json, "+"));
            break;

        case KEFIR_PUNCTUATOR_MINUS:
            REQUIRE_OK(kefir_json_output_string(json, "-"));
            break;

        case KEFIR_PUNCTUATOR_TILDE:
            REQUIRE_OK(kefir_json_output_string(json, "~"));
            break;

        case KEFIR_PUNCTUATOR_EXCLAMATION_MARK:
            REQUIRE_OK(kefir_json_output_string(json, "!"));
            break;

        case KEFIR_PUNCTUATOR_SLASH:
            REQUIRE_OK(kefir_json_output_string(json, "/"));
            break;

        case KEFIR_PUNCTUATOR_PERCENT:
            REQUIRE_OK(kefir_json_output_string(json, "%"));
            break;

        case KEFIR_PUNCTUATOR_LEFT_SHIFT:
            REQUIRE_OK(kefir_json_output_string(json, "<<"));
            break;

        case KEFIR_PUNCTUATOR_RIGHT_SHIFT:
            REQUIRE_OK(kefir_json_output_string(json, ">>"));
            break;

        case KEFIR_PUNCTUATOR_LESS_THAN:
            REQUIRE_OK(kefir_json_output_string(json, "<"));
            break;

        case KEFIR_PUNCTUATOR_GREATER_THAN:
            REQUIRE_OK(kefir_json_output_string(json, ">"));
            break;

        case KEFIR_PUNCTUATOR_LESS_OR_EQUAL:
            REQUIRE_OK(kefir_json_output_string(json, "<="));
            break;

        case KEFIR_PUNCTUATOR_GREATER_OR_EQUAL:
            REQUIRE_OK(kefir_json_output_string(json, ">="));
            break;

        case KEFIR_PUNCTUATOR_EQUAL:
            REQUIRE_OK(kefir_json_output_string(json, "=="));
            break;

        case KEFIR_PUNCTUATOR_NOT_EQUAL:
            REQUIRE_OK(kefir_json_output_string(json, "!="));
            break;

        case KEFIR_PUNCTUATOR_CARET:
            REQUIRE_OK(kefir_json_output_string(json, "^"));
            break;

        case KEFIR_PUNCTUATOR_VBAR:
            REQUIRE_OK(kefir_json_output_string(json, "|"));
            break;

        case KEFIR_PUNCTUATOR_DOUBLE_AMPERSAND:
            REQUIRE_OK(kefir_json_output_string(json, "&&"));
            break;

        case KEFIR_PUNCTUATOR_DOUBLE_VBAR:
            REQUIRE_OK(kefir_json_output_string(json, "||"));
            break;

        case KEFIR_PUNCTUATOR_QUESTION_MARK:
            REQUIRE_OK(kefir_json_output_string(json, "?"));
            break;

        case KEFIR_PUNCTUATOR_COLON:
            REQUIRE_OK(kefir_json_output_string(json, ":"));
            break;

        case KEFIR_PUNCTUATOR_SEMICOLON:
            REQUIRE_OK(kefir_json_output_string(json, ";"));
            break;

        case KEFIR_PUNCTUATOR_ELLIPSIS:
            REQUIRE_OK(kefir_json_output_string(json, "..."));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN:
            REQUIRE_OK(kefir_json_output_string(json, "="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_MULTIPLY:
            REQUIRE_OK(kefir_json_output_string(json, "*="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_DIVIDE:
            REQUIRE_OK(kefir_json_output_string(json, "/="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_MODULO:
            REQUIRE_OK(kefir_json_output_string(json, "%="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_ADD:
            REQUIRE_OK(kefir_json_output_string(json, "+="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_SUBTRACT:
            REQUIRE_OK(kefir_json_output_string(json, "-="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_SHIFT_LEFT:
            REQUIRE_OK(kefir_json_output_string(json, "<<="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_SHIFT_RIGHT:
            REQUIRE_OK(kefir_json_output_string(json, ">>="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_AND:
            REQUIRE_OK(kefir_json_output_string(json, "&="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_XOR:
            REQUIRE_OK(kefir_json_output_string(json, "^="));
            break;

        case KEFIR_PUNCTUATOR_ASSIGN_OR:
            REQUIRE_OK(kefir_json_output_string(json, "|="));
            break;

        case KEFIR_PUNCTUATOR_COMMA:
            REQUIRE_OK(kefir_json_output_string(json, ","));
            break;

        case KEFIR_PUNCTUATOR_HASH:
            REQUIRE_OK(kefir_json_output_string(json, "#"));
            break;

        case KEFIR_PUNCTUATOR_DOUBLE_HASH:
            REQUIRE_OK(kefir_json_output_string(json, "##"));
            break;

        case KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACKET:
            REQUIRE_OK(kefir_json_output_string(json, "<:"));
            break;

        case KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACKET:
            REQUIRE_OK(kefir_json_output_string(json, ":>"));
            break;

        case KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACE:
            REQUIRE_OK(kefir_json_output_string(json, "<%"));
            break;

        case KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACE:
            REQUIRE_OK(kefir_json_output_string(json, "%>"));
            break;

        case KEFIR_PUNCTUATOR_DIGRAPH_HASH:
            REQUIRE_OK(kefir_json_output_string(json, "%:"));
            break;

        case KEFIR_PUNCTUATOR_DIGRAPH_DOUBLE_HASH:
            REQUIRE_OK(kefir_json_output_string(json, "%:%:"));
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t format_constant(struct kefir_json_output *json, const struct kefir_constant_token *constant) {
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    switch (constant->type) {
        case KEFIR_CONSTANT_TOKEN_INTEGER:
            REQUIRE_OK(kefir_json_output_string(json, "integer"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_integer(json, constant->integer));
            break;

        case KEFIR_CONSTANT_TOKEN_LONG_INTEGER:
            REQUIRE_OK(kefir_json_output_string(json, "long_integer"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_integer(json, constant->integer));
            break;

        case KEFIR_CONSTANT_TOKEN_LONG_LONG_INTEGER:
            REQUIRE_OK(kefir_json_output_string(json, "long_long_integer"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_integer(json, constant->integer));
            break;

        case KEFIR_CONSTANT_TOKEN_UNSIGNED_INTEGER:
            REQUIRE_OK(kefir_json_output_string(json, "unsigned"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, constant->uinteger));
            break;

        case KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_INTEGER:
            REQUIRE_OK(kefir_json_output_string(json, "long_unsigned"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, constant->uinteger));
            break;

        case KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_LONG_INTEGER:
            REQUIRE_OK(kefir_json_output_string(json, "long_long_unsigned"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, constant->uinteger));
            break;

        case KEFIR_CONSTANT_TOKEN_FLOAT:
            REQUIRE_OK(kefir_json_output_string(json, "float"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_hexfloat(json, constant->float32));
            break;

        case KEFIR_CONSTANT_TOKEN_DOUBLE:
            REQUIRE_OK(kefir_json_output_string(json, "double"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_hexfloat(json, constant->float64));
            break;

        case KEFIR_CONSTANT_TOKEN_CHAR:
            REQUIRE_OK(kefir_json_output_string(json, "char"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_integer(json, constant->character));
            break;

        case KEFIR_CONSTANT_TOKEN_WIDE_CHAR:
            REQUIRE_OK(kefir_json_output_string(json, "wide_char"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_integer(json, constant->wide_char));
            break;

        case KEFIR_CONSTANT_TOKEN_UNICODE16_CHAR:
            REQUIRE_OK(kefir_json_output_string(json, "unicode16_char"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, constant->unicode16_char));
            break;

        case KEFIR_CONSTANT_TOKEN_UNICODE32_CHAR:
            REQUIRE_OK(kefir_json_output_string(json, "unicode32_char"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, constant->unicode32_char));
            break;
    }
    return KEFIR_OK;
}

kefir_result_t format_string_literal(struct kefir_json_output *json, const struct kefir_token *token) {
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    switch (token->string_literal.type) {
        case KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE:
            REQUIRE_OK(kefir_json_output_string(json, "multibyte"));
            REQUIRE_OK(kefir_json_output_object_key(json, "content"));
            REQUIRE_OK(kefir_json_output_raw_string(json, (const char *) token->string_literal.literal,
                                                    token->string_literal.length));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE8:
            REQUIRE_OK(kefir_json_output_string(json, "unicode8"));
            REQUIRE_OK(kefir_json_output_object_key(json, "content"));
            REQUIRE_OK(kefir_json_output_raw_string(json, (const char *) token->string_literal.literal,
                                                    token->string_literal.length));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE16: {
            REQUIRE_OK(kefir_json_output_string(json, "unicode16"));
            REQUIRE_OK(kefir_json_output_object_key(json, "content"));
            REQUIRE_OK(kefir_json_output_array_begin(json));
            const kefir_char16_t *literal = token->string_literal.literal;
            for (kefir_size_t i = 0; i < token->string_literal.length; i++) {
                REQUIRE_OK(kefir_json_output_uinteger(json, literal[i]));
            }
            REQUIRE_OK(kefir_json_output_array_end(json));
        } break;

        case KEFIR_STRING_LITERAL_TOKEN_UNICODE32:
            REQUIRE_OK(kefir_json_output_string(json, "unicode32"));
            REQUIRE_OK(kefir_json_output_object_key(json, "content"));
            REQUIRE_OK(kefir_json_output_array_begin(json));
            for (kefir_size_t i = 0; i < token->string_literal.length; i++) {
                REQUIRE_OK(
                    kefir_json_output_uinteger(json, ((const kefir_char32_t *) token->string_literal.literal)[i]));
            }
            REQUIRE_OK(kefir_json_output_array_end(json));
            break;

        case KEFIR_STRING_LITERAL_TOKEN_WIDE:
            REQUIRE_OK(kefir_json_output_string(json, "wide"));
            REQUIRE_OK(kefir_json_output_object_key(json, "content"));
            REQUIRE_OK(kefir_json_output_array_begin(json));
            for (kefir_size_t i = 0; i < token->string_literal.length; i++) {
                REQUIRE_OK(
                    kefir_json_output_uinteger(json, ((const kefir_wchar_t *) token->string_literal.literal)[i]));
            }
            REQUIRE_OK(kefir_json_output_array_end(json));
            break;
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "length"));
    REQUIRE_OK(kefir_json_output_uinteger(json, token->string_literal.length));
    return KEFIR_OK;
}

kefir_result_t kefir_token_format(struct kefir_json_output *json, const struct kefir_token *token,
                                  kefir_bool_t display_source_location) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid json output"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    switch (token->klass) {
        case KEFIR_TOKEN_SENTINEL:
            REQUIRE_OK(kefir_json_output_string(json, "sentinel"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, false));
            break;

        case KEFIR_TOKEN_KEYWORD:
            REQUIRE_OK(kefir_json_output_string(json, "keyword"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, false));
            REQUIRE_OK(format_keyword(json, token->keyword));
            break;

        case KEFIR_TOKEN_IDENTIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "identifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, false));
            REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
            REQUIRE_OK(kefir_json_output_string(json, token->identifier));
            break;

        case KEFIR_TOKEN_PUNCTUATOR:
            REQUIRE_OK(kefir_json_output_string(json, "punctuator"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, false));
            REQUIRE_OK(format_punctuator(json, token->punctuator));
            break;

        case KEFIR_TOKEN_STRING_LITERAL:
            REQUIRE_OK(kefir_json_output_string(json, "string_literal"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, false));
            REQUIRE_OK(format_string_literal(json, token));
            break;

        case KEFIR_TOKEN_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "constant"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, false));
            REQUIRE_OK(format_constant(json, &token->constant));
            break;

        case KEFIR_TOKEN_PP_WHITESPACE:
            REQUIRE_OK(kefir_json_output_string(json, "whitespace"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, true));
            REQUIRE_OK(kefir_json_output_object_key(json, "newline"));
            REQUIRE_OK(kefir_json_output_boolean(json, token->pp_whitespace.newline));
            break;

        case KEFIR_TOKEN_PP_NUMBER:
            REQUIRE_OK(kefir_json_output_string(json, "pp_number"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, true));
            REQUIRE_OK(kefir_json_output_object_key(json, "literal"));
            REQUIRE_OK(kefir_json_output_string(json, token->pp_number.number_literal));
            break;

        case KEFIR_TOKEN_PP_HEADER_NAME:
            REQUIRE_OK(kefir_json_output_string(json, "pp_header_name"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, true));
            REQUIRE_OK(kefir_json_output_object_key(json, "system_header"));
            REQUIRE_OK(kefir_json_output_boolean(json, token->pp_header_name.system));
            REQUIRE_OK(kefir_json_output_object_key(json, "header_name"));
            REQUIRE_OK(kefir_json_output_string(json, token->pp_header_name.header_name));
            break;

        case KEFIR_TOKEN_PP_PLACEMAKER:
            REQUIRE_OK(kefir_json_output_string(json, "pp_placemaker"));
            REQUIRE_OK(kefir_json_output_object_key(json, "preprocessor"));
            REQUIRE_OK(kefir_json_output_boolean(json, true));
            break;
    }

    if (display_source_location) {
        REQUIRE_OK(kefir_json_output_object_key(json, "source_location"));
        if (kefir_source_location_get(&token->source_location, NULL, NULL, NULL)) {
            REQUIRE_OK(kefir_json_output_object_begin(json));
            REQUIRE_OK(kefir_json_output_object_key(json, "source"));
            REQUIRE_OK(kefir_json_output_string(json, token->source_location.source));
            REQUIRE_OK(kefir_json_output_object_key(json, "line"));
            REQUIRE_OK(kefir_json_output_uinteger(json, token->source_location.line));
            REQUIRE_OK(kefir_json_output_object_key(json, "column"));
            REQUIRE_OK(kefir_json_output_uinteger(json, token->source_location.column));
            REQUIRE_OK(kefir_json_output_object_end(json));
        } else {
            REQUIRE_OK(kefir_json_output_null(json));
        }
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_token_buffer_format(struct kefir_json_output *json, const struct kefir_token_buffer *buffer,
                                         kefir_bool_t display_source_location) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid json output"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (kefir_size_t i = 0; i < buffer->length; i++) {
        REQUIRE_OK(kefir_token_format(json, &buffer->tokens[i], display_source_location));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}
