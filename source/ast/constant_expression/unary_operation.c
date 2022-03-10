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
#include "kefir/ast/type_completion.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

static kefir_result_t unwrap_vla_type(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                      const struct kefir_ast_type *type, const struct kefir_ast_type **result) {
    REQUIRE_OK(kefir_ast_type_completion(mem, context, &type, type));

    if (KEFIR_AST_TYPE_IS_VL_ARRAY(type)) {
        const struct kefir_ast_type *element_type = NULL;
        REQUIRE_OK(unwrap_vla_type(mem, context, type->array_type.element_type, &element_type));
        struct kefir_ast_constant_expression *len_expr = kefir_ast_constant_expression_integer(mem, 1);
        REQUIRE(len_expr != NULL,
                KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate integral constant expression"));
        const struct kefir_ast_type *array_type =
            kefir_ast_type_array(mem, context->type_bundle, element_type, len_expr, NULL);
        REQUIRE_ELSE(array_type != NULL, {
            kefir_ast_constant_expression_free(mem, len_expr);
            return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed toa allocate AST array type");
        });
        *result = array_type;
    } else {
        *result = type;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_evaluate_unary_operation_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_unary_operation *node,
                                                       struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST unary operation node"));
    REQUIRE(value != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Expected constant expression AST node"));
    REQUIRE(node->base.properties.expression_props.constant_expression,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Expected constant expression AST node"));

    struct kefir_ast_constant_expression_value arg_value;
    switch (node->type) {
        case KEFIR_AST_OPERATION_PLUS:
            REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg, &arg_value));
            if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                value->integer = arg_value.integer;
            } else if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                value->floating_point = arg_value.floating_point;
            } else {
                return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                              "Expected integeral or floating-point constant expression");
            }
            break;

        case KEFIR_AST_OPERATION_NEGATE:
            REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg, &arg_value));
            if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                value->integer = -arg_value.integer;
            } else if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                value->floating_point = -arg_value.floating_point;
            } else {
                return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                              "Expected integeral or floating-point constant expression");
            }
            break;

        case KEFIR_AST_OPERATION_INVERT:
            REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg, &arg_value));
            if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                value->integer = ~arg_value.integer;
            } else {
                return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->arg->source_location,
                                              "Expected integeral constant expression");
            }
            break;

        case KEFIR_AST_OPERATION_LOGICAL_NEGATE:
            REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg, &arg_value));
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                value->integer = !arg_value.integer;
            } else if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                value->integer = !arg_value.floating_point;
            } else {
                switch (arg_value.pointer.type) {
                    case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER:
                    case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_LITERAL:
                        value->integer = false;
                        break;

                    case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER:
                        value->integer = !(kefir_bool_t) (arg_value.pointer.base.integral + arg_value.pointer.offset);
                        break;
                }
            }
            break;

        case KEFIR_AST_OPERATION_POSTFIX_INCREMENT:
        case KEFIR_AST_OPERATION_POSTFIX_DECREMENT:
        case KEFIR_AST_OPERATION_PREFIX_INCREMENT:
        case KEFIR_AST_OPERATION_PREFIX_DECREMENT:
            return KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->base.source_location,
                                          "Constant expressions shall not contain increment/decrement operators");

        case KEFIR_AST_OPERATION_ADDRESS:
            REQUIRE_OK(kefir_ast_constant_expression_value_evaluate_lvalue_reference(mem, context, node->arg,
                                                                                     &value->pointer));
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS;
            break;

        case KEFIR_AST_OPERATION_INDIRECTION:
            return KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->base.source_location,
                                          "Constant expression cannot contain indirection operator");

        case KEFIR_AST_OPERATION_SIZEOF: {
            const struct kefir_ast_type *type = node->arg->properties.type;
            if (context->configuration->analysis.ext_pointer_arithmetics &&
                (type->tag == KEFIR_AST_TYPE_FUNCTION ||
                 kefir_ast_unqualified_type(type)->tag == KEFIR_AST_TYPE_VOID)) {
                type = context->type_traits->incomplete_type_substitute;
            }

            REQUIRE_OK(kefir_ast_type_completion(mem, context, &type, type));

            kefir_ast_target_environment_opaque_type_t opaque_type;
            struct kefir_ast_target_environment_object_info type_info;
            REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, context->target_env, type, &opaque_type));
            kefir_result_t res =
                KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, context->target_env, opaque_type, NULL, &type_info);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type);
                return res;
            });
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = type_info.size;
            REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type));
        } break;

        case KEFIR_AST_OPERATION_ALIGNOF: {
            kefir_ast_target_environment_opaque_type_t opaque_type;
            struct kefir_ast_target_environment_object_info type_info;
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(unwrap_vla_type(mem, context, node->arg->properties.type, &type));
            REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, context->target_env, type, &opaque_type));
            kefir_result_t res =
                KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, context->target_env, opaque_type, NULL, &type_info);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type);
                return res;
            });
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = type_info.alignment;
            REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type));
        } break;
    }
    return KEFIR_OK;
}
