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
#include "kefir/lexer/lexer.h"
#include "kefir/ast/format.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char SOURCE_CODE[] = "int ints[] = { 0, 1, 2, 3, [100] = 100 };\n"
                               "char string[] = {{{\"Hello world\"}}};\n"
                               "int ints[3][3] = {\n"
                               "    [0][0] = 1,\n"
                               "    [1][1] = 2,\n"
                               "    [2] = { 1, 2, 3 }\n"
                               "};";

    struct kefir_symbol_table symbols;
    struct kefir_lexer_source_cursor source_cursor;
    struct kefir_lexer_context parser_context;
    struct kefir_lexer lexer;
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_lexer_source_cursor_init(&source_cursor, SOURCE_CODE, sizeof(SOURCE_CODE), "<stdin>"));
    REQUIRE_OK(kefir_lexer_context_default(&parser_context));
    REQUIRE_OK(kefir_lexer_init(mem, &lexer, &symbols, &source_cursor, &parser_context, NULL));
    REQUIRE_OK(kefir_token_buffer_init(&tokens));
    REQUIRE_OK(kefir_lexer_populate_buffer(mem, &tokens, &lexer));
    REQUIRE_OK(kefir_lexer_free(mem, &lexer));

    struct kefir_parser_token_cursor cursor;
    struct kefir_parser parser;
    REQUIRE_OK(kefir_parser_token_cursor_init(&cursor, tokens.tokens, tokens.length));
    REQUIRE_OK(kefir_parser_init(mem, &parser, &symbols, &cursor, NULL));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, stdout, 4));

    struct kefir_ast_node_base *node = NULL;
    REQUIRE_OK(KEFIR_PARSER_NEXT_TRANSLATION_UNIT(mem, &parser, &node));
    REQUIRE_OK(kefir_ast_format(&json, node, true));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));

    REQUIRE_OK(kefir_json_output_finalize(&json));

    REQUIRE_OK(kefir_parser_free(mem, &parser));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
