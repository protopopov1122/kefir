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

#include "kefir/ast/constant_expression_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

kefir_result_t kefir_ast_evaluate_conditional_operator_node(struct kefir_mem *mem,
                                                            const struct kefir_ast_context *context,
                                                            const struct kefir_ast_conditional_operator *node,
                                                            struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST conditional operator node"));
    REQUIRE(value != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Expected constant expression AST node"));
    REQUIRE(node->base.properties.expression_props.constant_expression,
            KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->base.source_location,
                                   "Expected constant expression AST node"));

    struct kefir_ast_constant_expression_value cond_value, arg1_value, arg2_value;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->condition, &cond_value));
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->expr1, &arg1_value));
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->expr2, &arg2_value));

    kefir_bool_t condition = false;
    switch (cond_value.klass) {
        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
            condition = cond_value.integer;
            break;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
            condition = (kefir_bool_t) cond_value.floating_point;
            break;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
            switch (cond_value.pointer.type) {
                case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER:
                case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_LITERAL:
                    condition = true;
                    break;

                case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER:
                    condition = (kefir_bool_t) (cond_value.pointer.base.integral + cond_value.pointer.offset);
                    break;
            }
            break;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Non-evaluated constant expression");
    }

    if (condition) {
        *value = arg1_value;
    } else {
        *value = arg2_value;
    }
    return KEFIR_OK;
}
