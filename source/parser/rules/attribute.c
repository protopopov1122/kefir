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
#include "kefir/core/source_error.h"

static kefir_result_t consume_attribute_list_entry(struct kefir_mem *mem, struct kefir_parser *parser) {
    REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                   "Expected either identifier or closing parentheses"));
    REQUIRE_OK(PARSER_SHIFT(parser));

    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        kefir_bool_t consume_attribute_parameters = true;
        while (consume_attribute_parameters) {
            if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE)) {
                consume_attribute_parameters = false;
            } else {
                struct kefir_ast_node_base *expr = NULL;
                REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, assignment_expression, &expr));
                REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, expr));

                if (!PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE)) {
                    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA),
                            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                                   "Expected either comma, or right parenthese"));
                    REQUIRE_OK(PARSER_SHIFT(parser));
                }
            }
        }
        REQUIRE_OK(PARSER_SHIFT(parser));
    }
    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(attribute)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                      struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE(PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ATTRIBUTE),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match an attribute"));

    if (parser->configuration->fail_on_attributes) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                      "Attributes are not supported");
    }

    REQUIRE_OK(PARSER_SHIFT(parser));

    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected parenthese"));
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 1, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 1), "Expected parenthese"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(PARSER_SHIFT(parser));

    kefir_bool_t consume_attribute_list = true;
    while (consume_attribute_list) {
        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE) &&
            PARSER_TOKEN_IS_PUNCTUATOR(parser, 1, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE)) {
            consume_attribute_list = false;
        } else {
            REQUIRE_OK(consume_attribute_list_entry(mem, parser));

            if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
                REQUIRE_OK(PARSER_SHIFT(parser));
            }
        }
    }
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(PARSER_SHIFT(parser));

    *result = NULL;
    return KEFIR_OK;
}
