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

#ifndef CONSTANT_EXPRESSION_H_
#define CONSTANT_EXPRESSION_H_

#include <string.h>

#define ASSERT_INTEGER_CONST_EXPR(_mem, _context, _node, _value)                                   \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER);                        \
        ASSERT(value.integer == (_value));                                                         \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_FLOAT_CONST_EXPR(_mem, _context, _node, _value)                                     \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT);                          \
        ASSERT(DOUBLE_EQUALS(value.floating_point, (_value), DOUBLE_EPSILON));                     \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_IDENTIFIER_CONST_EXPR(_mem, _context, _node, _value, _offset)                       \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS);                        \
        ASSERT(value.pointer.type == KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER);             \
        ASSERT(strcmp(value.pointer.base.literal, (_value)) == 0);                                 \
        ASSERT(value.pointer.offset == (_offset));                                                 \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_LITERAL_CONST_EXPR(_mem, _context, _node, _value)                                   \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS);                        \
        ASSERT(value.pointer.type == KEFIR_AST_CONSTANT_EXPRESSION_POINTER_LITERAL);               \
        ASSERT(strcmp(value.pointer.base.literal, (_value)) == 0);                                 \
        ASSERT(value.pointer.offset == 0);                                                         \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_INTPTR_CONST_EXPR(_mem, _context, _node, _value, _offset)                           \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS);                        \
        ASSERT(value.pointer.type == KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER);               \
        ASSERT(value.pointer.base.integral == (_value));                                           \
        ASSERT(value.pointer.offset == (_offset));                                                 \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_CONST_EXPR_NOK(_mem, _context, _node)                                                \
    do {                                                                                            \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                            \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                                \
        struct kefir_ast_constant_expression_value value;                                           \
        ASSERT_NOK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                               \
    } while (0)

#endif
