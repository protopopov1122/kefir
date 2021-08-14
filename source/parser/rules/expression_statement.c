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

#include "kefir/parser/rule_helpers.h"

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(expression_statement)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                                 struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    struct kefir_ast_node_base *expression = NULL;
    if (!PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON)) {
        REQUIRE_OK(KEFIR_PARSER_NEXT_EXPRESSION(mem, parser, &expression));
    }

    kefir_result_t res = KEFIR_OK;
    REQUIRE_CHAIN_SET(&res, PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON),
                      KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected semicolon"));
    REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));

    REQUIRE_ELSE(res == KEFIR_OK, {
        if (expression != NULL) {
            KEFIR_AST_NODE_FREE(mem, expression);
        }
        return res;
    });
    struct kefir_ast_expression_statement *stmt = kefir_ast_new_expression_statement(mem, expression);
    REQUIRE_ELSE(stmt != NULL, {
        if (expression != NULL) {
            KEFIR_AST_NODE_FREE(mem, expression);
        }
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST expression statement");
    });
    *result = KEFIR_AST_NODE_BASE(stmt);
    return KEFIR_OK;
}
