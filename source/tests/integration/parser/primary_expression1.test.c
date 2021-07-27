/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

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

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
#define COUNT 3
    struct kefir_symbol_table symbols;
    struct kefir_token TOKENS[COUNT];
    struct kefir_token_array_stream array_stream;
    struct kefir_parser parser;

    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_token_array_stream_init(&array_stream, TOKENS, COUNT));

    const char MSG[] = "HELLO, WORLD!\n\n\0";
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "X", &TOKENS[0]));
    REQUIRE_OK(kefir_token_new_constant_int(100, &TOKENS[1]));
    REQUIRE_OK(kefir_token_new_string_literal(mem, MSG, sizeof(MSG), &TOKENS[2]));
    REQUIRE_OK(kefir_parser_init(mem, &parser, &symbols, &array_stream.stream));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, stdout, 4));
    REQUIRE_OK(kefir_json_output_array_begin(&json));

    struct kefir_ast_node_base *node = NULL;
    for (kefir_size_t i = 0; i < COUNT; i++) {
        REQUIRE_OK(kefir_parser_next(mem, &parser, &node));
        REQUIRE_OK(kefir_ast_format(&json, node));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    }

    REQUIRE_OK(kefir_json_output_array_end(&json));
    REQUIRE_OK(kefir_json_output_finalize(&json));

    REQUIRE_OK(kefir_parser_free(mem, &parser));
    REQUIRE_OK(array_stream.stream.free(mem, &array_stream.stream));
    for (kefir_size_t i = 0; i < COUNT; i++) {
        REQUIRE_OK(kefir_token_free(mem, &TOKENS[i]));
    }
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
