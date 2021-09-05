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

static kefir_result_t scan_subscript(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    REQUIRE_OK(PARSER_SHIFT(builder->parser));
    REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, expression), NULL));
    REQUIRE(PARSER_TOKEN_IS_RIGHT_BRACKET(builder->parser, 0),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, NULL, "Expected right bracket"));
    REQUIRE_OK(PARSER_SHIFT(builder->parser));
    return KEFIR_OK;
}

static kefir_result_t scan_argument_list(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    REQUIRE_OK(PARSER_SHIFT(builder->parser));
    while (!PARSER_TOKEN_IS_PUNCTUATOR(builder->parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE)) {
        REQUIRE_OK(
            kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_function_call_append(mem, builder));

        if (PARSER_TOKEN_IS_PUNCTUATOR(builder->parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
            REQUIRE_OK(PARSER_SHIFT(builder->parser));
        } else {
            REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(builder->parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, NULL, "Expected either comma, or right parenthese"));
        }
    }
    REQUIRE_OK(PARSER_SHIFT(builder->parser));
    return KEFIR_OK;
}

static kefir_result_t scan_member(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    kefir_bool_t direct = PARSER_TOKEN_IS_PUNCTUATOR(builder->parser, 0, KEFIR_PUNCTUATOR_DOT);
    REQUIRE_OK(PARSER_SHIFT(builder->parser));
    REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(builder->parser, 0),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, NULL, "Expected identifier"));
    const struct kefir_token *token = PARSER_CURSOR(builder->parser, 0);
    REQUIRE_OK(PARSER_SHIFT(builder->parser));
    REQUIRE_OK(kefir_parser_ast_builder_struct_member(mem, builder, direct, token->identifier));
    return KEFIR_OK;
}

static kefir_result_t scan_postfixes(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    struct kefir_parser *parser = builder->parser;
    kefir_bool_t scan_postfix = true;

    REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, primary_expression), NULL));
    do {
        if (PARSER_TOKEN_IS_LEFT_BRACKET(parser, 0)) {
            REQUIRE_OK(scan_subscript(mem, builder));
            REQUIRE_OK(kefir_parser_ast_builder_array_subscript(mem, builder));
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
            REQUIRE_OK(kefir_parser_ast_builder_function_call(mem, builder));
            REQUIRE_OK(scan_argument_list(mem, builder));
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_DOT) ||
                   PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_ARROW)) {
            REQUIRE_OK(scan_member(mem, builder));
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_DOUBLE_PLUS)) {
            REQUIRE_OK(PARSER_SHIFT(parser));
            REQUIRE_OK(kefir_parser_ast_builder_unary_operation(mem, builder, KEFIR_AST_OPERATION_POSTFIX_INCREMENT));
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_DOUBLE_MINUS)) {
            REQUIRE_OK(PARSER_SHIFT(parser));
            REQUIRE_OK(kefir_parser_ast_builder_unary_operation(mem, builder, KEFIR_AST_OPERATION_POSTFIX_DECREMENT));
        } else {
            scan_postfix = false;
        }
    } while (scan_postfix);
    return KEFIR_OK;
}

static kefir_result_t builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser AST builder"));

    kefir_result_t res =
        kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, compound_literal), NULL);
    if (res == KEFIR_NO_MATCH) {
        res = scan_postfixes(mem, builder);
    }
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(postfix_expression)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                               struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, builder_callback, NULL));
    return KEFIR_OK;
}
