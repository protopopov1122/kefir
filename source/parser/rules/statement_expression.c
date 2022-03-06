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
#include "kefir/parser/builder.h"
#include "kefir/core/source_error.h"

static kefir_result_t builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser AST builder"));
    struct kefir_parser *parser = builder->parser;

    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE) &&
                PARSER_TOKEN_IS_LEFT_BRACE(parser, 1) && parser->configuration->statement_expressions,
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match statement expression"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(PARSER_SHIFT(parser));

    REQUIRE_OK(kefir_parser_ast_builder_statement_expression(mem, builder));
    REQUIRE_OK(kefir_parser_scope_push_block(mem, &parser->scope));

    while (!PARSER_TOKEN_IS_RIGHT_BRACE(parser, 0)) {
        kefir_result_t res =
            kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, declaration), NULL);
        if (res == KEFIR_NO_MATCH) {
            res = kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, statement), NULL);
        }

        if (res == KEFIR_NO_MATCH) {
            return KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                          "Expected either declaration or statement");
        } else {
            REQUIRE_OK(res);
        }
        REQUIRE_OK(kefir_parser_ast_builder_statement_expression_append(mem, builder));
    }

    REQUIRE(PARSER_TOKEN_IS_RIGHT_BRACE(parser, 0) &&
                PARSER_TOKEN_IS_PUNCTUATOR(parser, 1, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                   "Expected right brace and right parenthese"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(kefir_parser_scope_pop_block(mem, &parser->scope));
    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(statement_expression)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                                 struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, builder_callback, NULL));
    return KEFIR_OK;
}
