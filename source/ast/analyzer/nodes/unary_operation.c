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

#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

kefir_result_t kefir_ast_analyze_unary_operation_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                      const struct kefir_ast_unary_operation *node,
                                                      struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST unary operation"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->arg));
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    switch (node->type) {
        case KEFIR_AST_OPERATION_PLUS:
        case KEFIR_AST_OPERATION_NEGATE: {
            REQUIRE(node->arg->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Unary operator operand shall be an arithmetic expression"));
            const struct kefir_ast_type *type1 =
                KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->arg->properties.type);
            REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Unary operator operand shall be an arithmetic expression"));
            if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1)) {
                base->properties.type = kefir_ast_type_int_promotion(context->type_traits, type1);
            } else {
                base->properties.type = type1;
            }
            base->properties.expression_props.constant_expression =
                node->arg->properties.expression_props.constant_expression;
        } break;

        case KEFIR_AST_OPERATION_INVERT: {
            REQUIRE(node->arg->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Inversion operand shall be an integral expression"));
            const struct kefir_ast_type *type1 =
                KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->arg->properties.type);
            REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Inversion operand shall be an integral expression"));
            base->properties.type = kefir_ast_type_int_promotion(context->type_traits, type1);
            base->properties.expression_props.constant_expression =
                node->arg->properties.expression_props.constant_expression;
        } break;

        case KEFIR_AST_OPERATION_LOGICAL_NEGATE: {
            REQUIRE(node->arg->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Logical negation operand shall be a scalar expression"));
            const struct kefir_ast_type *type1 =
                KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->arg->properties.type);
            REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(type1),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Logical negation operand shall be a scalar expression"));
            base->properties.type = kefir_ast_type_signed_int();
            base->properties.expression_props.constant_expression =
                node->arg->properties.expression_props.constant_expression;
        } break;

        case KEFIR_AST_OPERATION_PREFIX_INCREMENT:
        case KEFIR_AST_OPERATION_PREFIX_DECREMENT:
        case KEFIR_AST_OPERATION_POSTFIX_INCREMENT:
        case KEFIR_AST_OPERATION_POSTFIX_DECREMENT: {
            REQUIRE(node->arg->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Increment/decrement operator operand shall be a scalar lvalue"));
            REQUIRE(node->arg->properties.expression_props.lvalue,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Increment/decrement operator operand shall be a scalar lvalue"));
            const struct kefir_ast_type *type = node->arg->properties.type;
            REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(type),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Increment/decrement operator operand shall be a scalar lvalue"));
            base->properties.type = type;
        } break;

        case KEFIR_AST_OPERATION_ADDRESS: {
            REQUIRE(node->arg->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Address operator operand shall be an addrssable expression"));
            REQUIRE(node->arg->properties.expression_props.addressable,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Address operator operand shall be an addrssable expression"));
            REQUIRE_OK(kefir_ast_node_is_lvalue_reference_constant(
                context, node->arg, &base->properties.expression_props.constant_expression));
            const struct kefir_ast_type *type = node->arg->properties.type;
            base->properties.type = kefir_ast_type_pointer(mem, context->type_bundle, type);
        } break;

        case KEFIR_AST_OPERATION_INDIRECTION: {
            REQUIRE(node->arg->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Indirection operator operand shall be a pointer"));
            const struct kefir_ast_type *type =
                KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->arg->properties.type);
            REQUIRE(type->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Indirection operator operand shall be a pointer"));
            type = type->referenced_type;
            base->properties.type = type;
            if (type->tag != KEFIR_AST_TYPE_FUNCTION) {
                base->properties.expression_props.lvalue = true;
            }
            base->properties.expression_props.addressable = true;
        } break;

        case KEFIR_AST_OPERATION_SIZEOF: {
            REQUIRE(node->arg->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION ||
                        node->arg->properties.category == KEFIR_AST_NODE_CATEGORY_TYPE,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Sizeof operator expects expression or type name"));
            const struct kefir_ast_type *type = node->arg->properties.type;
            REQUIRE(!node->arg->properties.expression_props.bitfield,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Sizeof operator cannot be applied to bit-fields"));
            REQUIRE(type->tag != KEFIR_AST_TYPE_FUNCTION,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Sizeof operator cannot be applied to function types"));
            REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Sizeof operator cannot be applied to incomplete type"));
            base->properties.type = kefir_ast_type_signed_int();
            base->properties.expression_props.constant_expression = !KEFIR_AST_TYPE_IS_VL_ARRAY(type);
        } break;

        case KEFIR_AST_OPERATION_ALIGNOF: {
            REQUIRE(node->arg->properties.category == KEFIR_AST_NODE_CATEGORY_TYPE,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Alignof operator expects type argument"));
            const struct kefir_ast_type *type = node->arg->properties.type;
            REQUIRE(type->tag != KEFIR_AST_TYPE_FUNCTION,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Alignof operator cannot be applied to function types"));
            REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                           "Alignof operator cannot be applied to incomplete type"));
            base->properties.type = kefir_ast_type_signed_int();
            base->properties.expression_props.constant_expression = true;
        } break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected unary AST node type");
    }
    return KEFIR_OK;
}
