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

#define BITWISE_RULE(_id, _expr, _punctuator, _oper)                                                                 \
    static kefir_result_t _id##_builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder,    \
                                                 void *payload) {                                                    \
        UNUSED(payload);                                                                                             \
        REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));           \
        REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser AST builder"));     \
        struct kefir_parser *parser = builder->parser;                                                               \
                                                                                                                     \
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, _expr), NULL));          \
        while (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, (_punctuator))) {                                               \
            REQUIRE_OK(PARSER_SHIFT(parser));                                                                        \
            REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, _expr), NULL));      \
            REQUIRE_OK(kefir_parser_ast_builder_binary_operation(mem, builder, (_oper)));                            \
        }                                                                                                            \
        return KEFIR_OK;                                                                                             \
    }                                                                                                                \
                                                                                                                     \
    kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(bitwise_##_id##_expression)(                                          \
        struct kefir_mem * mem, struct kefir_parser * parser, struct kefir_ast_node_base * *result, void *payload) { \
        APPLY_PROLOGUE(mem, parser, result, payload);                                                                \
        REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, _id##_builder_callback, NULL));                \
        return KEFIR_OK;                                                                                             \
    }

BITWISE_RULE(and, equality_expression, KEFIR_PUNCTUATOR_AMPERSAND, KEFIR_AST_OPERATION_BITWISE_AND)
BITWISE_RULE(xor, bitwise_and_expression, KEFIR_PUNCTUATOR_CARET, KEFIR_AST_OPERATION_BITWISE_XOR)
BITWISE_RULE(or, bitwise_xor_expression, KEFIR_PUNCTUATOR_VBAR, KEFIR_AST_OPERATION_BITWISE_OR)
