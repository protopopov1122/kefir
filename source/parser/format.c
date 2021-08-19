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

#include "kefir/parser/format.h"
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

kefir_result_t kefir_token_format(struct kefir_json_output *json, const struct kefir_token *token) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid json output"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    switch (token->klass) {
        case KEFIR_TOKEN_SENTINEL:
            REQUIRE_OK(kefir_json_output_string(json, "sentinel"));
            break;

        case KEFIR_TOKEN_KEYWORD:
            REQUIRE_OK(kefir_json_output_string(json, "keyword"));
            REQUIRE_OK(format_keyword(json, token->keyword));
            break;

        case KEFIR_TOKEN_IDENTIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "identifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
            REQUIRE_OK(kefir_json_output_string(json, token->identifier));
            break;

        case KEFIR_TOKEN_CONSTANT:
        case KEFIR_TOKEN_STRING_LITERAL:
        case KEFIR_TOKEN_PUNCTUATOR:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Token JSON formatter is not implemented yet");
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}
