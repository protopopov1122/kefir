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

#include "kefir/parser/parser.h"
#include "kefir/test/unit_test.h"
#include "kefir/test/util.h"
#include "kefir/util/char32.h"

static kefir_result_t on_init(struct kefir_mem *mem, struct kefir_parser *parser) {
    parser->extension_payload = KEFIR_MALLOC(mem, 1024);
    return KEFIR_OK;
}

static kefir_result_t on_free(struct kefir_mem *mem, struct kefir_parser *parser) {
    KEFIR_FREE(mem, parser->extension_payload);
    parser->extension_payload = NULL;
    return KEFIR_OK;
}

DEFINE_CASE(parser_extensions1, "Parser - extensions #1") {
    struct kefir_symbol_table symbols;
    struct kefir_token TOKENS[1024];
    struct kefir_parser_token_cursor cursor;
    struct kefir_parser parser;
    struct kefir_parser_extensions extensions = {.on_init = on_init, .on_free = on_free};

    ASSERT_OK(kefir_symbol_table_init(&symbols));

    kefir_size_t counter = 0;
    ASSERT_OK(kefir_token_new_identifier(&kft_mem, &symbols, "idx", &TOKENS[counter++]));

    ASSERT_OK(kefir_parser_token_cursor_init(&cursor, TOKENS, counter));
    ASSERT_OK(kefir_parser_init(&kft_mem, &parser, &symbols, &cursor, &extensions));

    ASSERT(parser.extension_payload != NULL);
    struct kefir_ast_node_base *node = NULL;
    ASSERT_OK(KEFIR_PARSER_NEXT_EXPRESSION(&kft_mem, &parser, &node));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node));
    ASSERT_OK(kefir_parser_free(&kft_mem, &parser));
    for (kefir_size_t i = 0; i < counter; i++) {
        ASSERT_OK(kefir_token_free(&kft_mem, &TOKENS[i]));
    }
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
