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
#include "kefir/parser/parser.h"
#include "kefir/ast/format.h"
#include <stdio.h>

static kefir_result_t next_identifier(struct kefir_mem *mem, struct kefir_parser *parser,
                                      struct kefir_ast_node_base **result, void *payload) {
    const struct kefir_token *token = kefir_parser_token_cursor_at(parser->cursor, 0);
    if (token != NULL && token->klass == KEFIR_TOKEN_IDENTIFIER && strcmp(token->identifier, "@") == 0) {
        REQUIRE_OK(kefir_parser_token_cursor_next(parser->cursor));
        token = kefir_parser_token_cursor_at(parser->cursor, 0);
        if (token != NULL && token->klass == KEFIR_TOKEN_PUNCTUATOR &&
            token->punctuator == KEFIR_PUNCTUATOR_LEFT_PARENTHESE) {
            *result = KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, parser->symbols, "__runtime_entry_point"));
        } else {
            *result = KEFIR_AST_NODE_BASE(kefir_ast_new_function_call(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, parser->symbols, "__runtime_entry_point"))));
        }
    } else {
        REQUIRE_OK(parser->ruleset.rules[KEFIR_PARSER_RULESET_EXTRA_SLOT_OFFSET](mem, parser, result, payload));
    }
    return KEFIR_OK;
}

static kefir_result_t on_init(struct kefir_mem *mem, struct kefir_parser *parser) {
    UNUSED(mem);
    parser->ruleset.rules[KEFIR_PARSER_RULESET_EXTRA_SLOT_OFFSET] =
        parser->ruleset.rules[KEFIR_PARSER_RULESET_IDENTIFIER(identifier)];
    parser->ruleset.rules[KEFIR_PARSER_RULESET_IDENTIFIER(identifier)] = next_identifier;
    return KEFIR_OK;
}

static kefir_result_t on_free(struct kefir_mem *mem, struct kefir_parser *parser) {
    UNUSED(mem);
    parser->ruleset.rules[KEFIR_PARSER_RULESET_IDENTIFIER(identifier)] =
        parser->ruleset.rules[KEFIR_PARSER_RULESET_EXTRA_SLOT_OFFSET];
    parser->ruleset.rules[KEFIR_PARSER_RULESET_EXTRA_SLOT_OFFSET] = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_symbol_table symbols;
    struct kefir_token TOKENS[1024];
    struct kefir_parser_token_cursor cursor;
    struct kefir_parser parser;
    struct kefir_parser_extensions extensions = {.on_init = on_init, .on_free = on_free};

    REQUIRE_OK(kefir_symbol_table_init(&symbols));

    kefir_size_t counter = 0;

    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_SEMICOLON, &TOKENS[counter++]));

    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "@", &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "var1", &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_COMMA, &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "var2", &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_PLUS, &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "@", &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_SEMICOLON, &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "@", &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_BRACKET, &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "@", &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_BRACKET, &TOKENS[counter++]));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_SEMICOLON, &TOKENS[counter++]));

    REQUIRE_OK(kefir_parser_token_cursor_init(&cursor, TOKENS, counter));
    REQUIRE_OK(kefir_parser_init(mem, &parser, &symbols, &cursor, &extensions));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, stdout, 4));

    REQUIRE_OK(kefir_json_output_array_begin(&json));

    while (kefir_parser_token_cursor_at(&cursor, 0)->klass != KEFIR_TOKEN_SENTINEL) {
        struct kefir_ast_node_base *node = NULL;
        REQUIRE_OK(KEFIR_PARSER_NEXT_STATEMENT(mem, &parser, &node));
        REQUIRE_OK(kefir_ast_format(&json, node, false));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    }

    REQUIRE_OK(kefir_json_output_array_end(&json));
    REQUIRE_OK(kefir_json_output_finalize(&json));

    REQUIRE_OK(kefir_parser_free(mem, &parser));
    for (kefir_size_t i = 0; i < counter; i++) {
        REQUIRE_OK(kefir_token_free(mem, &TOKENS[i]));
    }
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
