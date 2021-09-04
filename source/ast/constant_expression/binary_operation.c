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
#include "kefir/core/lang_error.h"

#define ANY_OF(x, y, _klass) ((x)->klass == (_klass) || (y)->klass == (_klass))

static kefir_ast_constant_expression_float_t as_float(const struct kefir_ast_constant_expression_value *value) {
    switch (value->klass) {
        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
            return (kefir_ast_constant_expression_float_t) value->integer;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
            return value->floating_point;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
            return 0.0f;
    }
    return 0.0f;
}

static kefir_result_t evaluate_pointer_offset(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                              const struct kefir_ast_binary_operation *node,
                                              struct kefir_ast_constant_expression_pointer *pointer,
                                              kefir_ast_constant_expression_int_t index,
                                              struct kefir_ast_constant_expression_value *value) {
    kefir_ast_target_environment_opaque_type_t opaque_type;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, context->target_env,
                                                     node->base.properties.type->referenced_type, &opaque_type));
    kefir_int64_t offset = 0;
    kefir_result_t res =
        KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_OFFSET(mem, context->target_env, opaque_type, index, &offset);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type);
        return res;
    });
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type));

    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS;
    value->pointer = *pointer;
    value->pointer.offset += offset;
    value->pointer.pointer_node = KEFIR_AST_NODE_BASE(node);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_evaluate_binary_operation_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                        const struct kefir_ast_binary_operation *node,
                                                        struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST binary operation node"));
    REQUIRE(value != NULL,
            KEFIR_SET_LANG_ERROR(KEFIR_ANALYSIS_ERROR, NULL, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_LANG_ERROR(KEFIR_ANALYSIS_ERROR, NULL, "Expected constant expression AST node"));
    REQUIRE(node->base.properties.expression_props.constant_expression,
            KEFIR_SET_ERROR(KEFIR_NOT_CONSTANT, "Expected constant expression AST node"));

    struct kefir_ast_constant_expression_value arg1_value, arg2_value;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg1, &arg1_value));
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg2, &arg2_value));
    switch (node->type) {
        case KEFIR_AST_OPERATION_ADD:
            if (arg1_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS) {
                REQUIRE_OK(evaluate_pointer_offset(mem, context, node, &arg1_value.pointer, arg2_value.integer, value));
            } else if (arg2_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS) {
                REQUIRE_OK(evaluate_pointer_offset(mem, context, node, &arg2_value.pointer, arg1_value.integer, value));
            } else if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                value->floating_point = as_float(&arg1_value) + as_float(&arg2_value);
            } else {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                value->integer = arg1_value.integer + arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_SUBTRACT:
            if (arg1_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS) {
                REQUIRE(arg2_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                        KEFIR_SET_LANG_ERROR(KEFIR_ANALYSIS_ERROR, NULL,
                                             "Second subtraction operand shall have integral type"));
                REQUIRE_OK(
                    evaluate_pointer_offset(mem, context, node, &arg1_value.pointer, -arg2_value.integer, value));
            } else if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                value->floating_point = as_float(&arg1_value) - as_float(&arg2_value);
            } else {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                value->integer = arg1_value.integer - arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_MULTIPLY:
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                value->floating_point = as_float(&arg1_value) * as_float(&arg2_value);
            } else {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                value->integer = arg1_value.integer * arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_DIVIDE:
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                value->floating_point = as_float(&arg1_value) / as_float(&arg2_value);
            } else {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                value->integer = arg1_value.integer / arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_MODULO:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = arg1_value.integer % arg2_value.integer;
            break;

        case KEFIR_AST_OPERATION_SHIFT_LEFT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = arg1_value.integer << arg2_value.integer;
            break;

        case KEFIR_AST_OPERATION_SHIFT_RIGHT:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = arg1_value.integer >> arg2_value.integer;
            break;

        case KEFIR_AST_OPERATION_LESS:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS)) {
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                       "Constant expressions with address comparisons are not supported");
            } else if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->integer = as_float(&arg1_value) < as_float(&arg2_value);
            } else {
                value->integer = arg1_value.integer < arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_LESS_EQUAL:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS)) {
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                       "Constant expressions with address comparisons are not supported");
            } else if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->integer = as_float(&arg1_value) <= as_float(&arg2_value);
            } else {
                value->integer = arg1_value.integer <= arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_GREATER:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS)) {
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                       "Constant expressions with address comparisons are not supported");
            } else if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->integer = as_float(&arg1_value) > as_float(&arg2_value);
            } else {
                value->integer = arg1_value.integer > arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_GREATER_EQUAL:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS)) {
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                       "Constant expressions with address comparisons are not supported");
            } else if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->integer = as_float(&arg1_value) >= as_float(&arg2_value);
            } else {
                value->integer = arg1_value.integer >= arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_EQUAL:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS)) {
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                       "Constant expressions with address comparisons are not supported");
            } else if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->integer = as_float(&arg1_value) == as_float(&arg2_value);
            } else {
                value->integer = arg1_value.integer == arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_NOT_EQUAL:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS)) {
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                       "Constant expressions with address comparisons are not supported");
            } else if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT)) {
                value->integer = as_float(&arg1_value) != as_float(&arg2_value);
            } else {
                value->integer = arg1_value.integer != arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_BITWISE_AND:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = arg1_value.integer & arg2_value.integer;
            break;

        case KEFIR_AST_OPERATION_BITWISE_OR:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = arg1_value.integer | arg2_value.integer;
            break;

        case KEFIR_AST_OPERATION_BITWISE_XOR:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = arg1_value.integer ^ arg2_value.integer;
            break;

        case KEFIR_AST_OPERATION_LOGICAL_AND:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS)) {
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                       "Constant logical expressions with addresses are not supported");
            } else if (arg1_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT &&
                       arg2_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                value->integer = arg1_value.floating_point && arg2_value.floating_point;
            } else if (arg1_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                value->integer = arg1_value.floating_point && arg2_value.integer;
            } else if (arg2_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                value->integer = arg1_value.integer && arg2_value.floating_point;
            } else {
                value->integer = arg1_value.integer && arg2_value.integer;
            }
            break;

        case KEFIR_AST_OPERATION_LOGICAL_OR:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            if (ANY_OF(&arg1_value, &arg2_value, KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS)) {
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                       "Constant logical expressions with addresses are not supported");
            } else if (arg1_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT &&
                       arg2_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                value->integer = arg1_value.floating_point || arg2_value.floating_point;
            } else if (arg1_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                value->integer = arg1_value.floating_point || arg2_value.integer;
            } else if (arg2_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                value->integer = arg1_value.integer || arg2_value.floating_point;
            } else {
                value->integer = arg1_value.integer || arg2_value.integer;
            }
            break;
    }
    return KEFIR_OK;
}
