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

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(identifier)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                       struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0), KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Expected identifier token"));
    kefir_bool_t is_typedef = false;
    kefir_result_t res =
        kefir_parser_scope_is_typedef(&parser->scope, PARSER_CURSOR(parser, 0)->identifier, &is_typedef);
    if (res != KEFIR_NOT_FOUND) {
        REQUIRE_OK(res);
    }
    REQUIRE(!is_typedef, KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match identifier"));
    REQUIRE_ALLOC(
        result,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, parser->symbols, PARSER_CURSOR(parser, 0)->identifier)),
        "Failed to allocate AST identifier");
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}
