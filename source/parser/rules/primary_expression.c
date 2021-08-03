/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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

#include "kefir/parser/rule_helpers.h"

kefir_result_t KEFIR_PARSER_RULE_FN(primary_expression)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                        struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    kefir_result_t res = KEFIR_PARSER_RULE_APPLY(mem, parser, identifier, result);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = KEFIR_PARSER_RULE_APPLY(mem, parser, constant, result);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = KEFIR_PARSER_RULE_APPLY(mem, parser, string_literal, result);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = KEFIR_PARSER_RULE_APPLY(mem, parser, generic_selection, result);
    REQUIRE(res == KEFIR_NO_MATCH, res);

    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, expression, result));
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}
