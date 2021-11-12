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

#include "kefir/core/mem.h"
#include "kefir/core/error.h"
#include "kefir/lexer/lexer.h"
#include "kefir/lexer/format.h"
#include "kefir/ast/format.h"
#include "kefir/util/json.h"
#include <stdio.h>

static kefir_result_t ext_free(struct kefir_mem *mem, struct kefir_token *token) {
    UNUSED(mem);
    token->extension.klass = NULL;
    token->extension.payload = NULL;
    return KEFIR_OK;
}

static kefir_result_t ext_copy(struct kefir_mem *mem, struct kefir_token *dst, const struct kefir_token *src) {
    UNUSED(mem);
    REQUIRE_OK(kefir_token_new_extension(src->extension.klass, NULL, dst));
    return KEFIR_OK;
}

static kefir_result_t ext_format_json(struct kefir_json_output *json, const struct kefir_token *token,
                                      kefir_bool_t display_loc) {
    UNUSED(token);
    UNUSED(display_loc);
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    REQUIRE_OK(kefir_json_output_string(json, "at"));
    return KEFIR_OK;
}

static kefir_result_t ext_format(FILE *out, const struct kefir_token *token) {
    UNUSED(token);
    fprintf(out, "@");
    return KEFIR_OK;
}

static kefir_result_t ext_concat(struct kefir_mem *mem, const struct kefir_token *left, const struct kefir_token *right,
                                 struct kefir_token *result) {
    UNUSED(mem);
    UNUSED(left);
    UNUSED(right);
    UNUSED(result);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Concatenation is not supported");
}

static struct kefir_token_extension_class EXT_CLASS = {.free = ext_free,
                                                       .copy = ext_copy,
                                                       .concat = ext_concat,
                                                       .format = ext_format,
                                                       .format_json = ext_format_json,
                                                       .payload = NULL};

static kefir_result_t failed_lex(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token) {
    UNUSED(mem);
    if (kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'@') {
        REQUIRE_OK(kefir_token_new_extension(&EXT_CLASS, NULL, token));
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
        return KEFIR_OK;
    }
    return KEFIR_NO_MATCH;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char CONTENT[] = "20+@/3-@at(),test=@1";

    struct kefir_lexer_extensions extensions = {.failed_token_lex = failed_lex};

    struct kefir_symbol_table symbols;
    struct kefir_lexer_source_cursor cursor;
    struct kefir_lexer_context parser_context;
    struct kefir_lexer lexer;
    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, sizeof(CONTENT), ""));
    REQUIRE_OK(kefir_lexer_context_default(&parser_context));
    REQUIRE_OK(kefir_lexer_init(mem, &lexer, &symbols, &cursor, &parser_context, &extensions));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, stdout, 4));
    REQUIRE_OK(kefir_json_output_array_begin(&json));

    kefir_bool_t scan_tokens = true;
    while (scan_tokens) {
        struct kefir_token token;
        REQUIRE_OK(kefir_lexer_next(mem, &lexer, &token));
        REQUIRE_OK(kefir_token_format(&json, &token, false));
        scan_tokens = token.klass != KEFIR_TOKEN_SENTINEL;
        REQUIRE_OK(kefir_token_free(mem, &token));
    }

    REQUIRE_OK(kefir_json_output_array_end(&json));
    REQUIRE_OK(kefir_json_output_finalize(&json));

    REQUIRE_OK(kefir_lexer_free(mem, &lexer));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
