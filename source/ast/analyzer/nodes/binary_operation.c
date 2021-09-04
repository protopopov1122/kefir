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

static kefir_result_t analyze_modulo(const struct kefir_ast_context *context, const struct kefir_ast_type *type1,
                                     const struct kefir_ast_type *type2, struct kefir_ast_node_base *base) {
    REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1) && KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type2),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both modulo operands shall have integral type"));
    base->properties.type = kefir_ast_type_common_arithmetic(context->type_traits, type1, type2);
    REQUIRE(base->properties.type != NULL,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unable to determine common AST arithmetic type"));
    return KEFIR_OK;
}

static kefir_result_t analyze_muldiv(const struct kefir_ast_context *context, const struct kefir_ast_type *type1,
                                     const struct kefir_ast_type *type2, struct kefir_ast_node_base *base) {
    REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1) && KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type2),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both multiply/divide operands shall have arithmetic type"));
    base->properties.type = kefir_ast_type_common_arithmetic(context->type_traits, type1, type2);
    REQUIRE(base->properties.type != NULL,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unable to determine common AST arithmetic type"));
    return KEFIR_OK;
}

static kefir_result_t analyze_addition(const struct kefir_ast_context *context, const struct kefir_ast_type *type1,
                                       const struct kefir_ast_type *type2, struct kefir_ast_node_base *base) {
    if (type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER && !KEFIR_AST_TYPE_IS_INCOMPLETE(type1->referenced_type)) {
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type2),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Second operand shall be have an integral type"));
        base->properties.type = type1;
    } else if (type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER && !KEFIR_AST_TYPE_IS_INCOMPLETE(type2->referenced_type)) {
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "First operand shall be have an integral type"));
        base->properties.type = type2;
    } else {
        REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1) && KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type2),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both operands shall have arithmetic types"));
        base->properties.type = kefir_ast_type_common_arithmetic(context->type_traits, type1, type2);
        REQUIRE(base->properties.type != NULL,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unable to determine common AST arithmetic type"));
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_subtraction(const struct kefir_ast_context *context, const struct kefir_ast_type *type1,
                                          const struct kefir_ast_type *type2, struct kefir_ast_node_base *base) {
    if (type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER && type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        const struct kefir_ast_type *obj_type1 = kefir_ast_unqualified_type(type1->referenced_type);
        const struct kefir_ast_type *obj_type2 = kefir_ast_unqualified_type(type2->referenced_type);
        REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(obj_type1) && !KEFIR_AST_TYPE_IS_INCOMPLETE(obj_type2),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both operands shall have complete types"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, obj_type1, obj_type2),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both operands shall have compatible types"));
        base->properties.type = context->type_traits->ptrdiff_type;
    } else if (type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER && !KEFIR_AST_TYPE_IS_INCOMPLETE(type1->referenced_type)) {
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type2),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Right operand shal have an integral type"));
        base->properties.type = type1;
    } else {
        REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1) && KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type2),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both operands shall have arithmetic types"));
        base->properties.type = kefir_ast_type_common_arithmetic(context->type_traits, type1, type2);
        REQUIRE(base->properties.type != NULL,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unable to determine common AST arithmetic type"));
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_shift(const struct kefir_ast_context *context, const struct kefir_ast_type *type1,
                                    const struct kefir_ast_type *type2, struct kefir_ast_node_base *base) {
    REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1) && KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type2),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Bitwise shift operator expects integer arguments"));
    base->properties.type = kefir_ast_type_int_promotion(context->type_traits, type1);
    REQUIRE(base->properties.type != NULL,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unable to determine common AST arithmetic type"));
    return KEFIR_OK;
}

static kefir_result_t analyze_relational(const struct kefir_ast_context *context, const struct kefir_ast_type *type1,
                                         const struct kefir_ast_type *type2, struct kefir_ast_node_base *base) {
    if (type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER && type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        const struct kefir_ast_type *obj_type1 = kefir_ast_unqualified_type(type1->referenced_type);
        const struct kefir_ast_type *obj_type2 = kefir_ast_unqualified_type(type2->referenced_type);
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, obj_type1, obj_type2),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both operands shall have compatible types"));
    } else {
        REQUIRE(KEFIR_AST_TYPE_IS_REAL_TYPE(type1) && KEFIR_AST_TYPE_IS_REAL_TYPE(type2),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both operands shall have real types"));
    }
    base->properties.type = kefir_ast_type_signed_int();
    return KEFIR_OK;
}

static kefir_result_t analyze_equality(const struct kefir_ast_context *context, const struct kefir_ast_node_base *node1,
                                       const struct kefir_ast_type *type1, const struct kefir_ast_node_base *node2,
                                       const struct kefir_ast_type *type2, struct kefir_ast_node_base *base) {
    if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1) && KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type2)) {
        base->properties.type = kefir_ast_type_signed_int();
    } else if (type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER && type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        const struct kefir_ast_type *unqualified1 = kefir_ast_unqualified_type(type1->referenced_type);
        const struct kefir_ast_type *unqualified2 = kefir_ast_unqualified_type(type2->referenced_type);
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, unqualified1, unqualified2) ||
                    unqualified1->tag == KEFIR_AST_TYPE_VOID || unqualified2->tag == KEFIR_AST_TYPE_VOID,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both pointer operands shall point to compatible types or void"));
        base->properties.type = kefir_ast_type_signed_int();
    } else if (type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type2) && node2->properties.expression_props.constant_expression,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Second operand shall be NULL pointer"));
        base->properties.type = kefir_ast_type_signed_int();
    } else {
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1) && node1->properties.expression_props.constant_expression,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "First operand shall be NULL pointer"));
        REQUIRE(type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Second operand shall be pointer"));
        base->properties.type = kefir_ast_type_signed_int();
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_bitwise(const struct kefir_ast_context *context, const struct kefir_ast_type *type1,
                                      const struct kefir_ast_type *type2, struct kefir_ast_node_base *base) {
    REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1) && KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type2),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both operands shall have integral types"));
    base->properties.type = kefir_ast_type_common_arithmetic(context->type_traits, type1, type2);
    REQUIRE(base->properties.type != NULL,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unable to determine common AST arithmetic type"));
    return KEFIR_OK;
}

static kefir_result_t analyze_logical(const struct kefir_ast_context *context, const struct kefir_ast_type *type1,
                                      const struct kefir_ast_type *type2, struct kefir_ast_node_base *base) {
    UNUSED(context);
    REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(type1) && KEFIR_AST_TYPE_IS_SCALAR_TYPE(type2),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both operands shall have scalar types"));
    base->properties.type = kefir_ast_type_signed_int();
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_binary_operation_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_binary_operation *node,
                                                       struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST binary operation"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->arg1));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->arg2));

    REQUIRE(node->arg1->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both assignment operands shall be expressions"));
    REQUIRE(node->arg2->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both binary operator operands shall be expressions"));

    const struct kefir_ast_type *type1 =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->arg1->properties.type);
    const struct kefir_ast_type *type2 =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->arg2->properties.type);
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.expression_props.constant_expression =
        node->arg1->properties.expression_props.constant_expression &&
        node->arg2->properties.expression_props.constant_expression;
    switch (node->type) {
        case KEFIR_AST_OPERATION_MODULO:
            REQUIRE_OK(analyze_modulo(context, type1, type2, base));
            break;

        case KEFIR_AST_OPERATION_MULTIPLY:
        case KEFIR_AST_OPERATION_DIVIDE:
            REQUIRE_OK(analyze_muldiv(context, type1, type2, base));
            break;

        case KEFIR_AST_OPERATION_ADD:
            REQUIRE_OK(analyze_addition(context, type1, type2, base));
            break;

        case KEFIR_AST_OPERATION_SUBTRACT:
            REQUIRE_OK(analyze_subtraction(context, type1, type2, base));
            break;

        case KEFIR_AST_OPERATION_SHIFT_LEFT:
        case KEFIR_AST_OPERATION_SHIFT_RIGHT:
            REQUIRE_OK(analyze_shift(context, type1, type2, base));
            break;

        case KEFIR_AST_OPERATION_LESS:
        case KEFIR_AST_OPERATION_LESS_EQUAL:
        case KEFIR_AST_OPERATION_GREATER:
        case KEFIR_AST_OPERATION_GREATER_EQUAL:
            REQUIRE_OK(analyze_relational(context, type1, type2, base));
            break;

        case KEFIR_AST_OPERATION_EQUAL:
        case KEFIR_AST_OPERATION_NOT_EQUAL:
            REQUIRE_OK(analyze_equality(context, node->arg1, type1, node->arg2, type2, base));
            break;

        case KEFIR_AST_OPERATION_BITWISE_AND:
        case KEFIR_AST_OPERATION_BITWISE_OR:
        case KEFIR_AST_OPERATION_BITWISE_XOR:
            REQUIRE_OK(analyze_bitwise(context, type1, type2, base));
            break;

        case KEFIR_AST_OPERATION_LOGICAL_AND:
        case KEFIR_AST_OPERATION_LOGICAL_OR:
            REQUIRE_OK(analyze_logical(context, type1, type2, base));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected AST binary operator");
    }
    return KEFIR_OK;
}
