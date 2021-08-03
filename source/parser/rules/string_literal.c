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
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t KEFIR_PARSER_RULE_FN(string_literal)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                    struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE(PARSER_TOKEN_IS_STRING_LITERAL(parser, 0),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Expected string literal token"));
    const struct kefir_token *token = PARSER_CURSOR(parser, 0);
    REQUIRE_ALLOC(result,
                  KEFIR_AST_NODE_BASE(
                      kefir_ast_new_string_literal(mem, token->string_literal.content, token->string_literal.length)),
                  "Failed to allocate AST string literal");
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}
