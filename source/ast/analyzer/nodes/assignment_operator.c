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

static kefir_result_t validate_simple_assignment(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                 const struct kefir_ast_assignment_operator *node) {
    const struct kefir_ast_type *target_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->target->properties.type);

    kefir_result_t res;
    REQUIRE_MATCH_OK(&res, kefir_ast_node_assignable(mem, context, node->value, target_type),
                     KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->value->source_location,
                                            "Expression shall be assignable to target type"));
    REQUIRE_OK(res);
    return KEFIR_OK;
}

static kefir_result_t validate_compound_assignment(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                   const struct kefir_ast_assignment_operator *node) {
    const struct kefir_ast_type *target_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->target->properties.type);
    const struct kefir_ast_type *value_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->value->properties.type);
    switch (node->operation) {
        case KEFIR_AST_ASSIGNMENT_MULTIPLY:
        case KEFIR_AST_ASSIGNMENT_DIVIDE:
            REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(target_type),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->target->source_location,
                                           "Expected assignment target operand to have arithmetic type"));
            REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(value_type),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->value->source_location,
                                           "Expected assignment value operand to have arithmetic type"));
            break;

        case KEFIR_AST_ASSIGNMENT_MODULO:
        case KEFIR_AST_ASSIGNMENT_SHIFT_LEFT:
        case KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT:
        case KEFIR_AST_ASSIGNMENT_BITWISE_AND:
        case KEFIR_AST_ASSIGNMENT_BITWISE_OR:
        case KEFIR_AST_ASSIGNMENT_BITWISE_XOR:
            REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(target_type),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->target->source_location,
                                           "Expected assignment target operand have integral type"));
            REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(value_type),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->value->source_location,
                                           "Expected assignment value operand to have integral type"));
            break;

        case KEFIR_AST_ASSIGNMENT_ADD:
        case KEFIR_AST_ASSIGNMENT_SUBTRACT:
            if (target_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
                REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(kefir_ast_unqualified_type(target_type->referenced_type)),
                        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->target->source_location,
                                               "Assignment target operand shall be a pointer to complete type"));
                REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(value_type),
                        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->value->source_location,
                                               "Assignment value operand shall have integral type"));
            } else {
                REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(target_type),
                        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->target->source_location,
                                               "Expected assignment target operand to have arithmetic type"));
                REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(value_type),
                        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->value->source_location,
                                               "Expected assignment value operand to have arithmetic type"));
            }
            break;

        case KEFIR_AST_ASSIGNMENT_SIMPLE:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected assignment type");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_assignment_operator_node(struct kefir_mem *mem,
                                                          const struct kefir_ast_context *context,
                                                          const struct kefir_ast_assignment_operator *node,
                                                          struct kefir_ast_node_base *base) {
    UNUSED(mem);
    UNUSED(context);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->target));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->value));

    REQUIRE(node->target->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->target->source_location,
                                   "Both assignment operands shall be expressions"));
    REQUIRE(node->value->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->value->source_location,
                                   "Both assignment operands shall be expressions"));

    REQUIRE(node->target->properties.expression_props.lvalue,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->target->source_location,
                                   "Expected non-const lvalue as assignment target operand"));
    struct kefir_ast_type_qualification target_qualifications;
    REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&target_qualifications, node->target->properties.type));
    REQUIRE(!target_qualifications.constant,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->target->source_location,
                                   "Expected non-const lvalue as assignment target operand"));

    switch (node->operation) {
        case KEFIR_AST_ASSIGNMENT_SIMPLE:
            REQUIRE_OK(validate_simple_assignment(mem, context, node));
            break;

        default:
            REQUIRE_OK(validate_compound_assignment(mem, context, node));
            break;
    }

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->target->properties.type);
    return KEFIR_OK;
}
