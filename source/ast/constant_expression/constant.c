/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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

#include "kefir/ast/constant_expression_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

kefir_result_t kefir_ast_evaluate_scalar_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                              const struct kefir_ast_constant *node,
                                              struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant node"));
    REQUIRE(value != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Expected constant expression AST node"));
    REQUIRE(node->base.properties.expression_props.constant_expression,
            KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->base.source_location,
                                   "Expected constant expression AST node"));

    switch (node->type) {
        case KEFIR_AST_BOOL_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = node->value.boolean;
            break;

        case KEFIR_AST_CHAR_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = node->value.character;
            break;

        case KEFIR_AST_WIDE_CHAR_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = node->value.wide_character;
            break;

        case KEFIR_AST_UNICODE16_CHAR_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = node->value.unicode16_character;
            break;

        case KEFIR_AST_UNICODE32_CHAR_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = node->value.unicode32_character;
            break;

        case KEFIR_AST_INT_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = node->value.integer;
            break;

        case KEFIR_AST_UINT_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->uinteger = node->value.uinteger;
            break;

        case KEFIR_AST_LONG_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = node->value.long_integer;
            break;

        case KEFIR_AST_ULONG_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->uinteger = node->value.ulong_integer;
            break;

        case KEFIR_AST_LONG_LONG_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = node->value.long_long;
            break;

        case KEFIR_AST_ULONG_LONG_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->uinteger = node->value.ulong_long;
            break;

        case KEFIR_AST_FLOAT_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
            value->floating_point = node->value.float32;
            break;

        case KEFIR_AST_DOUBLE_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
            value->floating_point = node->value.float64;
            break;

        case KEFIR_AST_LONG_DOUBLE_CONSTANT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
            value->floating_point = node->value.long_double;
            break;
    }
    return KEFIR_OK;
}
