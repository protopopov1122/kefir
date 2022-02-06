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

#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/value.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast-translator/util.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t store_value(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                  struct kefir_irbuilder_block *builder,
                                  const struct kefir_ast_assignment_operator *node,
                                  const struct kefir_ast_type *result_normalized_type) {
    if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(result_normalized_type)) {
        // [V*, V1, V2]
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 1));  // [V*, V2, V1]
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 2));  // [V1, V2, V*]
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 2));  // [V1, V2, V*, V1]
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 2));  // [V1, V2, V*, V1, V2]
    } else {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 1));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 1));
    }
    REQUIRE_OK(kefir_ast_translator_store_lvalue(mem, context, builder, node->target));
    return KEFIR_OK;
}

static kefir_result_t translate_simple(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                       struct kefir_irbuilder_block *builder,
                                       const struct kefir_ast_assignment_operator *node) {
    const struct kefir_ast_type *value_init_normalized_type =
        kefir_ast_translator_normalize_type(node->value->properties.type);
    const struct kefir_ast_type *value_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, value_init_normalized_type);
    REQUIRE(value_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->target));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->value, builder, context));
    if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(result_normalized_type)) {
        REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, value_normalized_type,
                                                result_normalized_type));
    }
    REQUIRE_OK(store_value(mem, context, builder, node, result_normalized_type));
    return KEFIR_OK;
}

static kefir_result_t translate_multiplication(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                               struct kefir_irbuilder_block *builder,
                                               const struct kefir_ast_assignment_operator *node) {
    const struct kefir_ast_type *target_normalized_type =
        kefir_ast_translator_normalize_type(node->target->properties.type);
    const struct kefir_ast_type *value_init_normalized_type =
        kefir_ast_translator_normalize_type(node->value->properties.type);
    const struct kefir_ast_type *value_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, value_init_normalized_type);
    REQUIRE(value_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    const struct kefir_ast_type *common_type = kefir_ast_type_common_arithmetic(
        context->ast_context->type_traits, target_normalized_type, value_normalized_type);
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->target));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(kefir_ast_translator_resolve_lvalue(mem, context, builder, node->target));
    REQUIRE_OK(
        kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, target_normalized_type, common_type));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->value, builder, context));
    REQUIRE_OK(
        kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, value_normalized_type, common_type));
    switch (common_type->tag) {
        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDMUL, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64MUL, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32MUL, 0));
            break;

        default:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IMUL, 0));
            break;
    }
    REQUIRE_OK(
        kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, common_type, result_normalized_type));
    REQUIRE_OK(store_value(mem, context, builder, node, result_normalized_type));
    return KEFIR_OK;
}

static kefir_result_t translate_divide(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                       struct kefir_irbuilder_block *builder,
                                       const struct kefir_ast_assignment_operator *node) {
    const struct kefir_ast_type *target_normalized_type =
        kefir_ast_translator_normalize_type(node->target->properties.type);
    const struct kefir_ast_type *value_init_normalized_type =
        kefir_ast_translator_normalize_type(node->value->properties.type);
    const struct kefir_ast_type *value_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, value_init_normalized_type);
    REQUIRE(value_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    const struct kefir_ast_type *common_type = kefir_ast_type_common_arithmetic(
        context->ast_context->type_traits, target_normalized_type, value_normalized_type);
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->target));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(kefir_ast_translator_resolve_lvalue(mem, context, builder, node->target));
    REQUIRE_OK(
        kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, target_normalized_type, common_type));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->value, builder, context));
    REQUIRE_OK(
        kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, value_normalized_type, common_type));
    switch (common_type->tag) {
        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDDIV, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64DIV, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32DIV, 0));
            break;

        default: {
            kefir_bool_t signed_integer = false;
            REQUIRE_OK(kefir_ast_type_is_signed(context->ast_context->type_traits, common_type, &signed_integer));
            if (signed_integer) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IDIV, 0));
            } else {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_UDIV, 0));
            }
        } break;
    }
    REQUIRE_OK(
        kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, common_type, result_normalized_type));
    REQUIRE_OK(store_value(mem, context, builder, node, result_normalized_type));
    return KEFIR_OK;
}

static kefir_result_t translate_modulo_bitwise(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                               struct kefir_irbuilder_block *builder,
                                               const struct kefir_ast_assignment_operator *node) {
    const struct kefir_ast_type *target_normalized_type =
        kefir_ast_translator_normalize_type(node->target->properties.type);
    const struct kefir_ast_type *value_init_normalized_type =
        kefir_ast_translator_normalize_type(node->value->properties.type);
    const struct kefir_ast_type *value_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, value_init_normalized_type);
    REQUIRE(value_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    const struct kefir_ast_type *common_type = kefir_ast_type_common_arithmetic(
        context->ast_context->type_traits, target_normalized_type, value_normalized_type);
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->target));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(kefir_ast_translator_resolve_lvalue(mem, context, builder, node->target));
    REQUIRE_OK(
        kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, target_normalized_type, common_type));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->value, builder, context));
    REQUIRE_OK(
        kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, value_normalized_type, common_type));

    switch (node->operation) {
        case KEFIR_AST_ASSIGNMENT_MODULO:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IMOD, 0));
            break;

        case KEFIR_AST_ASSIGNMENT_SHIFT_LEFT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ILSHIFT, 0));
            break;

        case KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT: {
            kefir_bool_t signedness;
            REQUIRE_OK(kefir_ast_type_is_signed(context->ast_context->type_traits, value_normalized_type, &signedness));

            if (signedness) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IARSHIFT, 0));
            } else {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IRSHIFT, 0));
            }
        } break;

        case KEFIR_AST_ASSIGNMENT_BITWISE_AND:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IAND, 0));
            break;

        case KEFIR_AST_ASSIGNMENT_BITWISE_OR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IOR, 0));
            break;

        case KEFIR_AST_ASSIGNMENT_BITWISE_XOR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IXOR, 0));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected assignment operation");
    }

    REQUIRE_OK(
        kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, common_type, result_normalized_type));
    REQUIRE_OK(store_value(mem, context, builder, node, result_normalized_type));
    return KEFIR_OK;
}

static kefir_result_t translate_add(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                    struct kefir_irbuilder_block *builder,
                                    const struct kefir_ast_assignment_operator *node) {
    const struct kefir_ast_type *target_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, node->target->properties.type);
    const struct kefir_ast_type *value_init_normalized_type =
        kefir_ast_translator_normalize_type(node->value->properties.type);
    const struct kefir_ast_type *value_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, value_init_normalized_type);
    REQUIRE(value_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->target));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(kefir_ast_translator_resolve_lvalue(mem, context, builder, node->target));

    if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(target_normalized_type) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(value_normalized_type)) {
        const struct kefir_ast_type *common_type = kefir_ast_type_common_arithmetic(
            context->ast_context->type_traits, target_normalized_type, value_normalized_type);

        REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, target_normalized_type,
                                                common_type));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->value, builder, context));
        REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, value_normalized_type,
                                                common_type));
        switch (common_type->tag) {
            case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDADD, 0));
                break;

            case KEFIR_AST_TYPE_SCALAR_DOUBLE:
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64ADD, 0));
                break;

            case KEFIR_AST_TYPE_SCALAR_FLOAT:
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32ADD, 0));
                break;

            default:
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IADD, 0));
                break;
        }
        REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, common_type,
                                                result_normalized_type));
    } else {
        REQUIRE(target_normalized_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER &&
                    KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(value_normalized_type),
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                                "Expected scalar pointer on the left side, and an integer on the right"));

        const struct kefir_ast_translator_resolved_type *cached_type = NULL;
        REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(
            mem, &context->type_cache.resolver, context->environment, context->module,
            target_normalized_type->referenced_type, 0, &cached_type));
        REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected cached type to be an object"));

        REQUIRE_OK(kefir_ast_translate_expression(mem, node->value, builder, context));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_ELEMENTPTR, cached_type->object.ir_type_id,
                                                   cached_type->object.layout->value));
    }

    REQUIRE_OK(store_value(mem, context, builder, node, result_normalized_type));
    return KEFIR_OK;
}

static kefir_result_t translate_subtract(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                         struct kefir_irbuilder_block *builder,
                                         const struct kefir_ast_assignment_operator *node) {
    const struct kefir_ast_type *target_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, node->target->properties.type);
    const struct kefir_ast_type *value_init_normalized_type =
        kefir_ast_translator_normalize_type(node->value->properties.type);
    const struct kefir_ast_type *value_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, value_init_normalized_type);
    REQUIRE(value_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->target));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(kefir_ast_translator_resolve_lvalue(mem, context, builder, node->target));

    if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(target_normalized_type) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(value_normalized_type)) {
        const struct kefir_ast_type *common_type = kefir_ast_type_common_arithmetic(
            context->ast_context->type_traits, target_normalized_type, value_normalized_type);

        REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, target_normalized_type,
                                                common_type));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->value, builder, context));
        REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, value_normalized_type,
                                                common_type));
        switch (common_type->tag) {
            case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDSUB, 0));
                break;

            case KEFIR_AST_TYPE_SCALAR_DOUBLE:
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64SUB, 0));
                break;

            case KEFIR_AST_TYPE_SCALAR_FLOAT:
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32SUB, 0));
                break;

            default:
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ISUB, 0));
                break;
        }
        REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, common_type,
                                                result_normalized_type));
    } else {
        REQUIRE(target_normalized_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER &&
                    KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(value_normalized_type),
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                                "Expected scalar pointer on the left side, and an integer on the right"));

        const struct kefir_ast_translator_resolved_type *cached_type = NULL;
        REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(
            mem, &context->type_cache.resolver, context->environment, context->module,
            target_normalized_type->referenced_type, 0, &cached_type));
        REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected cached type to be an object"));

        REQUIRE_OK(kefir_ast_translate_expression(mem, node->value, builder, context));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INEG, 0));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_ELEMENTPTR, cached_type->object.ir_type_id,
                                                   cached_type->object.layout->value));
    }

    REQUIRE_OK(store_value(mem, context, builder, node, result_normalized_type));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_assignment_operator_node(struct kefir_mem *mem,
                                                            struct kefir_ast_translator_context *context,
                                                            struct kefir_irbuilder_block *builder,
                                                            const struct kefir_ast_assignment_operator *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant node"));

    switch (node->operation) {
        case KEFIR_AST_ASSIGNMENT_SIMPLE:
            REQUIRE_OK(translate_simple(mem, context, builder, node));
            break;

        case KEFIR_AST_ASSIGNMENT_MULTIPLY:
            REQUIRE_OK(translate_multiplication(mem, context, builder, node));
            break;

        case KEFIR_AST_ASSIGNMENT_DIVIDE:
            REQUIRE_OK(translate_divide(mem, context, builder, node));
            break;

        case KEFIR_AST_ASSIGNMENT_MODULO:
        case KEFIR_AST_ASSIGNMENT_SHIFT_LEFT:
        case KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT:
        case KEFIR_AST_ASSIGNMENT_BITWISE_AND:
        case KEFIR_AST_ASSIGNMENT_BITWISE_OR:
        case KEFIR_AST_ASSIGNMENT_BITWISE_XOR:
            REQUIRE_OK(translate_modulo_bitwise(mem, context, builder, node));
            break;

        case KEFIR_AST_ASSIGNMENT_ADD:
            REQUIRE_OK(translate_add(mem, context, builder, node));
            break;

        case KEFIR_AST_ASSIGNMENT_SUBTRACT:
            REQUIRE_OK(translate_subtract(mem, context, builder, node));
            break;
    }
    return KEFIR_OK;
}
