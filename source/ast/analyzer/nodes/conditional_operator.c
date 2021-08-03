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

kefir_result_t kefir_ast_analyze_conditional_operator_node(struct kefir_mem *mem,
                                                           const struct kefir_ast_context *context,
                                                           const struct kefir_ast_conditional_operator *node,
                                                           struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST conditional"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->condition));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->expr1));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->expr2));

    REQUIRE(node->condition->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All conditional operator operands shall be expressions"));
    REQUIRE(node->expr1->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All conditional operator operands shall be expressions"));
    REQUIRE(node->expr2->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All conditional operator operands shall be expressions"));

    const struct kefir_ast_type *cond_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->condition->properties.type);
    const struct kefir_ast_type *type1 =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->expr1->properties.type);
    const struct kefir_ast_type *type2 =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->expr2->properties.type);

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.expression_props.constant_expression =
        node->condition->properties.expression_props.constant_expression &&
        node->expr1->properties.expression_props.constant_expression &&
        node->expr2->properties.expression_props.constant_expression;

    REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(cond_type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "First operand shall have scalar type"));

    if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1) && KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type2)) {
        base->properties.type = kefir_ast_type_common_arithmetic(context->type_traits, type1, type2);
    } else if ((type1->tag == KEFIR_AST_TYPE_STRUCTURE || type1->tag == KEFIR_AST_TYPE_UNION) &&
               (type2->tag == KEFIR_AST_TYPE_STRUCTURE || type2->tag == KEFIR_AST_TYPE_UNION) &&
               KEFIR_AST_TYPE_SAME(type1, type2)) {
        base->properties.type = type1;
    } else if (type1->tag == KEFIR_AST_TYPE_VOID && type2->tag == KEFIR_AST_TYPE_VOID) {
        base->properties.type = kefir_ast_type_void();
    } else if (type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER && type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        struct kefir_ast_type_qualification qualifications1, qualifications2, total_qualifications;
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications1, type1->referenced_type));
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications2, type2->referenced_type));
        REQUIRE_OK(kefir_ast_type_merge_qualifications(&total_qualifications, &qualifications1, &qualifications2));
        const struct kefir_ast_type *unqualified1 = kefir_ast_unqualified_type(type1->referenced_type);
        const struct kefir_ast_type *unqualified2 = kefir_ast_unqualified_type(type2->referenced_type);

        if (unqualified1->tag == KEFIR_AST_TYPE_VOID || unqualified2->tag == KEFIR_AST_TYPE_VOID) {
            const struct kefir_ast_type *result_type = kefir_ast_type_void();
            if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&total_qualifications)) {
                result_type = kefir_ast_type_qualified(mem, context->type_bundle, result_type, total_qualifications);
            }
            base->properties.type = kefir_ast_type_pointer(mem, context->type_bundle, result_type);
        } else {
            REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, unqualified1, unqualified2),
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both conditional expressions shall have compatible types"));
            const struct kefir_ast_type *result_type =
                KEFIR_AST_TYPE_COMPOSITE(mem, context->type_bundle, context->type_traits, unqualified1, unqualified2);
            if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&total_qualifications)) {
                result_type = kefir_ast_type_qualified(mem, context->type_bundle, result_type, total_qualifications);
            }
            base->properties.type = kefir_ast_type_pointer(mem, context->type_bundle, result_type);
        }

    } else if (type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type2) && node->expr2->properties.expression_props.constant_expression,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Second conditional expression shall be a NULL pointer"));
        base->properties.type = type1;
    } else {
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1) && node->expr1->properties.expression_props.constant_expression,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "First conditional expression shall be a NULL pointer"));
        REQUIRE(type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Second conditional expression shall be a pointer"));
        base->properties.type = type2;
    }
    return KEFIR_OK;
}
