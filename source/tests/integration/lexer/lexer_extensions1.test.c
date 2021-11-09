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
#include "kefir/lexer/lexer.h"
#include "kefir/lexer/format.h"
#include "kefir/ast/format.h"
#include <stdio.h>

static kefir_result_t before_lex(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token) {
    UNUSED(token);
    REQUIRE(lexer->extension_payload != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_lexer_cursor_match_whitespace(mem, lexer, NULL));
    while (kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'_') {
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    }
    return KEFIR_OK;
}

static kefir_result_t failed_lex(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token) {
    UNUSED(mem);
    if (kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'@') {
        REQUIRE_OK(kefir_token_new_constant_char('@', token));
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
        return KEFIR_OK;
    }
    return KEFIR_NO_MATCH;
}

static kefir_result_t after_lex(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token) {
    UNUSED(lexer);
    if (token->klass == KEFIR_TOKEN_CONSTANT && token->constant.type == KEFIR_CONSTANT_TOKEN_INTEGER &&
        token->constant.integer == 0) {
        REQUIRE_OK(kefir_token_free(mem, token));
        REQUIRE_OK(kefir_token_new_constant_int(1, token));
    }
    return KEFIR_OK;
}

static kefir_result_t lexer_init(struct kefir_mem *mem, struct kefir_lexer *lexer) {
    lexer->extension_payload = KEFIR_MALLOC(mem, 100);
    return KEFIR_OK;
}

static kefir_result_t lexer_free(struct kefir_mem *mem, struct kefir_lexer *lexer) {
    KEFIR_FREE(mem, lexer->extension_payload);
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char CONTENT[] = "2+___3*4+@/fn(0);\n_@-000";

    struct kefir_lexer_extensions extensions = {.on_init = lexer_init,
                                                .on_free = lexer_free,
                                                .before_token_lex = before_lex,
                                                .failed_token_lex = failed_lex,
                                                .after_token_lex = after_lex};

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
