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

static kefir_result_t cast_integral_type(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const struct kefir_ast_type *type,
                                         struct kefir_ast_constant_expression_value *value,
                                         kefir_ast_constant_expression_int_t source) {
    kefir_ast_target_environment_opaque_type_t opaque_type;
    struct kefir_ast_target_environment_object_info type_info;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, context->target_env, type, &opaque_type));
    kefir_result_t res =
        KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, context->target_env, opaque_type, NULL, &type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type);
        return res;
    });
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type));

    kefir_bool_t signed_integer = false;
    REQUIRE_OK(kefir_ast_type_is_signed(context->type_traits, type, &signed_integer));

    if (type->tag == KEFIR_AST_TYPE_SCALAR_BOOL) {
        value->integer = (bool) source;
    } else if (signed_integer) {
        switch (type_info.size) {
            case 1:
                value->integer = (kefir_int8_t) source;
                break;

            case 2:
                value->integer = (kefir_int16_t) source;
                break;

            case 3:
            case 4:
                value->integer = (kefir_int32_t) source;
                break;

            default:
                value->integer = source;
                break;
        }
    } else {
        switch (type_info.size) {
            case 1:
                value->uinteger = (kefir_uint8_t) source;
                break;

            case 2:
                value->uinteger = (kefir_uint16_t) source;
                break;

            case 3:
            case 4:
                value->uinteger = (kefir_uint32_t) source;
                break;

            default:
                value->uinteger = source;
                break;
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_constant_expression_value_cast(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                        struct kefir_ast_constant_expression_value *value,
                                                        const struct kefir_ast_constant_expression_value *source,
                                                        const struct kefir_ast_node_base *node,
                                                        const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(value != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant expression value pointer"));
    REQUIRE(source != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source AST constant expression value"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid base AST node"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid destination AST type"));

    const struct kefir_ast_type *unqualified = kefir_ast_unqualified_type(type);
    if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(unqualified)) {
        value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
        switch (source->klass) {
            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
                REQUIRE_OK(cast_integral_type(mem, context, unqualified, value, source->integer));
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
                if (unqualified->tag == KEFIR_AST_TYPE_SCALAR_BOOL) {
                    value->integer = (bool) source->floating_point;
                } else {
                    REQUIRE_OK(cast_integral_type(mem, context, unqualified, value,
                                                  (kefir_ast_constant_expression_int_t) source->floating_point));
                }
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS;
                value->pointer = source->pointer;
                value->pointer.pointer_node = node;
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
                return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Non-evaluated constant expression");
        }
    } else if (KEFIR_AST_TYPE_IS_FLOATING_POINT(unqualified)) {
        value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
        switch (source->klass) {
            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
                value->floating_point = (kefir_ast_constant_expression_float_t) source->integer;
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
                value->floating_point = source->floating_point;
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->source_location,
                                              "Address to floating point cast is not a constant expression");

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
                return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Non-evaluated constant expression");
        }
    } else if (unqualified->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        REQUIRE(source->klass != KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->source_location,
                                       "Cannot cast floating point to address"));

        value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS;
        switch (source->klass) {
            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
                value->pointer.type = KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER;
                value->pointer.base.integral = source->integer;
                value->pointer.offset = 0;
                value->pointer.pointer_node = node;
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->source_location,
                                              "Unable to cast floating point to address");

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
                value->pointer = source->pointer;
                value->pointer.pointer_node = node;
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
                return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Non-evaluated constant expression");
        }
    } else {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->source_location, "Expected constant expression");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_evaluate_cast_operator_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     const struct kefir_ast_cast_operator *node,
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

    struct kefir_ast_constant_expression_value arg_value;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->expr, &arg_value));

    REQUIRE_OK(kefir_ast_constant_expression_value_cast(mem, context, value, &arg_value, KEFIR_AST_NODE_BASE(node),
                                                        node->type_name->base.properties.type));
    return KEFIR_OK;
}
