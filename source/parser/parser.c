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
#include "kefir/parser/rules.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_parser_init(struct kefir_mem *mem, struct kefir_parser *parser, struct kefir_symbol_table *symbols,
                                 struct kefir_parser_token_cursor *cursor) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token cursor"));

    REQUIRE_OK(kefir_parser_scope_init(mem, &parser->scope, symbols));
    parser->symbols = symbols;
    parser->cursor = cursor;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_free(struct kefir_mem *mem, struct kefir_parser *parser) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));

    REQUIRE_OK(kefir_parser_scope_free(mem, &parser->scope));
    parser->cursor = NULL;
    parser->symbols = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_apply(struct kefir_mem *mem, struct kefir_parser *parser,
                                  struct kefir_ast_node_base **result, kefir_parser_rule_fn_t rule, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST node"));
    REQUIRE(rule != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser rule"));

    kefir_size_t checkpoint;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));
    struct kefir_source_location source_location = kefir_parser_token_cursor_at(parser->cursor, 0)->source_location;
    kefir_result_t res = rule(mem, parser, result, payload);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));
        return res;
    } else {
        REQUIRE_OK(res);
        if (*result != NULL) {
            (*result)->source_location = source_location;
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_parser_try_invoke(struct kefir_mem *mem, struct kefir_parser *parser,
                                       kefir_parser_invocable_fn_t function, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(function != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser invocable"));

    kefir_size_t checkpoint;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));
    kefir_result_t res = function(mem, parser, payload);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));
        return res;
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}
