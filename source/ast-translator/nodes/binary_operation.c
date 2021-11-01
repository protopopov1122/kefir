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
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast-translator/util.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t binary_prologue(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                      struct kefir_irbuilder_block *builder,
                                      const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *arg1_init_normalized_type =
        kefir_ast_translator_normalize_type(node->arg1->properties.type);
    const struct kefir_ast_type *arg1_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, arg1_init_normalized_type);
    REQUIRE(arg1_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    const struct kefir_ast_type *arg2_init_normalized_type =
        kefir_ast_translator_normalize_type(node->arg2->properties.type);
    const struct kefir_ast_type *arg2_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, arg2_init_normalized_type);
    REQUIRE(arg2_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
    REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, arg1_normalized_type,
                                            result_normalized_type));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
    REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, arg2_normalized_type,
                                            result_normalized_type));
    return KEFIR_OK;
}

static kefir_result_t translate_addition(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                         struct kefir_irbuilder_block *builder,
                                         const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *arg1_normalized_type =
        kefir_ast_translator_normalize_type(node->arg1->properties.type);
    const struct kefir_ast_type *arg2_normalized_type =
        kefir_ast_translator_normalize_type(node->arg2->properties.type);
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg1_normalized_type) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg2_normalized_type)) {
        REQUIRE_OK(binary_prologue(mem, context, builder, node));
        switch (result_normalized_type->tag) {
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
    } else {
        const struct kefir_ast_translator_resolved_type *cached_type = NULL;
        REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(
            mem, &context->type_cache.resolver, context->environment, context->module,
            result_normalized_type->referenced_type, 0, &cached_type));
        REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected cached type to be an object"));

        if (arg1_normalized_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
            REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
            REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
        } else {
            REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
            REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
        }
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_ELEMENTPTR, cached_type->object.ir_type_id,
                                                   cached_type->object.layout->value));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_subtraction(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                            struct kefir_irbuilder_block *builder,
                                            const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *arg1_normalized_type =
        kefir_ast_translator_normalize_type(node->arg1->properties.type);
    const struct kefir_ast_type *arg2_normalized_type =
        kefir_ast_translator_normalize_type(node->arg2->properties.type);
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg1_normalized_type) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg2_normalized_type)) {
        switch (result_normalized_type->tag) {
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
    } else if (arg2_normalized_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        kefir_ast_target_environment_opaque_type_t opaque_type;
        struct kefir_ast_target_environment_object_info type_info;
        REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, &context->environment->target_env,
                                                         arg1_normalized_type->referenced_type, &opaque_type));
        kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, &context->environment->target_env,
                                                                      opaque_type, NULL, &type_info);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type);
            return res;
        });
        REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type));

        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ISUB, 0));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, type_info.size));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IDIV, 0));
    } else {
        const struct kefir_ast_translator_resolved_type *cached_type = NULL;
        REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(
            mem, &context->type_cache.resolver, context->environment, context->module,
            arg1_normalized_type->referenced_type, 0, &cached_type));
        REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected cached type to be an object"));

        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INEG, 0));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_ELEMENTPTR, cached_type->object.ir_type_id,
                                                   cached_type->object.layout->value));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_multiplication(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                               struct kefir_irbuilder_block *builder,
                                               const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    switch (result_normalized_type->tag) {
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
    return KEFIR_OK;
}

static kefir_result_t translate_division(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                         struct kefir_irbuilder_block *builder,
                                         const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *result_normalized_type =
        kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    switch (result_normalized_type->tag) {
        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDDIV, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64DIV, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32DIV, 0));
            break;

        default:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IDIV, 0));
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t translate_modulo(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                       struct kefir_irbuilder_block *builder,
                                       const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IMOD, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_bitwise(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                        struct kefir_irbuilder_block *builder,
                                        const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    const struct kefir_ast_type *arg1_normalized_type =
        kefir_ast_translator_normalize_type(node->arg1->properties.type);

    switch (node->type) {
        case KEFIR_AST_OPERATION_SHIFT_LEFT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ILSHIFT, 0));
            break;

        case KEFIR_AST_OPERATION_SHIFT_RIGHT: {
            kefir_bool_t signedness;
            REQUIRE_OK(kefir_ast_type_is_signed(context->ast_context->type_traits, arg1_normalized_type, &signedness));

            if (signedness) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IARSHIFT, 0));
            } else {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IRSHIFT, 0));
            }
        } break;

        case KEFIR_AST_OPERATION_BITWISE_AND:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IAND, 0));
            break;

        case KEFIR_AST_OPERATION_BITWISE_OR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IOR, 0));
            break;

        case KEFIR_AST_OPERATION_BITWISE_XOR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IXOR, 0));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected bitwise operation");
    }
    return KEFIR_OK;
}

static kefir_result_t translate_relational_equals(const struct kefir_ast_type *common_type,
                                                  struct kefir_irbuilder_block *builder) {
    switch (common_type->tag) {
        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDEQUALS, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64EQUALS, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32EQUALS, 0));
            break;

        default:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IEQUALS, 0));
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t translate_relational_not_equals(const struct kefir_ast_type *common_type,
                                                      struct kefir_irbuilder_block *builder) {
    switch (common_type->tag) {
        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDEQUALS, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64EQUALS, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32EQUALS, 0));
            break;

        default:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IEQUALS, 0));
            break;
    }
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_relational_less(const struct kefir_ast_type_traits *type_traits,
                                                const struct kefir_ast_type *common_type,
                                                struct kefir_irbuilder_block *builder) {
    switch (common_type->tag) {
        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDLESSER, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64LESSER, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32LESSER, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_POINTER:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IBELOW, 0));
            break;

        default: {
            kefir_bool_t signedness;
            REQUIRE_OK(kefir_ast_type_is_signed(type_traits, common_type, &signedness));

            if (signedness) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ILESSER, 0));
            } else {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IBELOW, 0));
            }
        } break;
    }
    return KEFIR_OK;
}

static kefir_result_t translate_relational_greater(const struct kefir_ast_type_traits *type_traits,
                                                   const struct kefir_ast_type *common_type,
                                                   struct kefir_irbuilder_block *builder) {
    switch (common_type->tag) {
        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDGREATER, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64GREATER, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32GREATER, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_POINTER:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IABOVE, 0));
            break;

        default: {
            kefir_bool_t signedness;
            REQUIRE_OK(kefir_ast_type_is_signed(type_traits, common_type, &signedness));

            if (signedness) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IGREATER, 0));
            } else {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IABOVE, 0));
            }
        } break;
    }
    return KEFIR_OK;
}

static kefir_result_t translate_relational_equality(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                    struct kefir_irbuilder_block *builder,
                                                    const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *arg1_init_normalized_type =
        kefir_ast_translator_normalize_type(node->arg1->properties.type);
    const struct kefir_ast_type *arg1_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, arg1_init_normalized_type);
    REQUIRE(arg1_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    const struct kefir_ast_type *arg2_init_normalized_type =
        kefir_ast_translator_normalize_type(node->arg2->properties.type);
    const struct kefir_ast_type *arg2_normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, arg2_init_normalized_type);
    REQUIRE(arg2_normalized_type != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));

    const struct kefir_ast_type *common_type = NULL;
    if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg1_normalized_type) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg2_normalized_type)) {
        common_type = kefir_ast_type_common_arithmetic(context->ast_context->type_traits, arg1_normalized_type,
                                                       arg2_normalized_type);
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
        REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, arg1_normalized_type,
                                                common_type));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
        REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, arg2_normalized_type,
                                                common_type));
    } else {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
        common_type = arg1_normalized_type;
    }

    switch (node->type) {
        case KEFIR_AST_OPERATION_EQUAL:
            REQUIRE_OK(translate_relational_equals(common_type, builder));
            break;

        case KEFIR_AST_OPERATION_NOT_EQUAL:
            REQUIRE_OK(translate_relational_not_equals(common_type, builder));
            break;

        case KEFIR_AST_OPERATION_LESS:
            REQUIRE_OK(translate_relational_less(context->ast_context->type_traits, common_type, builder));
            break;

        case KEFIR_AST_OPERATION_GREATER:
            REQUIRE_OK(translate_relational_greater(context->ast_context->type_traits, common_type, builder));
            break;

        case KEFIR_AST_OPERATION_LESS_EQUAL:
            if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(common_type)) {
                // Initial: [A1, A2, B1, B2]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 3));  // [A1, A2, B1, B2, A1]
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 3));  // [A1, A2, B1, B2, A1, A2]
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 3));  // [A1, A2, B1, B2, A1, A2, B1]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK,
                                                           3));  // [A1, A2, B1, B2, A1, A2, B1, B2]
                REQUIRE_OK(                                      // [A1, A2, B1, B2, A < B]
                    translate_relational_less(context->ast_context->type_traits, common_type, builder));
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 4));  // [A < B, A2, B1, B2, A1]
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 3));  // [A < B, A2, B1, B2, A1, A2]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK,
                                                           3));  // [A < B, A2, B1, B2, A1, A2, B1]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK,
                                                           3));                 // [A < B, A2, B1, B2, A1, A2, B1, B2]
                REQUIRE_OK(translate_relational_equals(common_type, builder));  // [A < B, A2, B1, B2, A == B]
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_DROP, 1));  // [A < B, A2, B1, A == B]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_DROP, 1));  // [A < B, A2, A == B]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_DROP, 1));  // [A < B, A == B]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BOR, 0));   // [A < B || A == B]
            } else {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 1));
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 1));
                REQUIRE_OK(translate_relational_less(context->ast_context->type_traits, common_type, builder));
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 2));
                REQUIRE_OK(translate_relational_equals(common_type, builder));
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BOR, 0));
            }
            break;

        case KEFIR_AST_OPERATION_GREATER_EQUAL:
            if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(common_type)) {
                // Initial: [A1, A2, B1, B2]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 3));  // [A1, A2, B1, B2, A1]
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 3));  // [A1, A2, B1, B2, A1, A2]
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 3));  // [A1, A2, B1, B2, A1, A2, B1]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK,
                                                           3));  // [A1, A2, B1, B2, A1, A2, B1, B2]
                REQUIRE_OK(                                      // [A1, A2, B1, B2, A > B]
                    translate_relational_greater(context->ast_context->type_traits, common_type, builder));
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 4));  // [A > B, A2, B1, B2, A1]
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 3));  // [A > B, A2, B1, B2, A1, A2]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK,
                                                           3));  // [A > B, A2, B1, B2, A1, A2, B1]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK,
                                                           3));                 // [A > B, A2, B1, B2, A1, A2, B1, B2]
                REQUIRE_OK(translate_relational_equals(common_type, builder));  // [A > B, A2, B1, B2, A == B]
                REQUIRE_OK(
                    KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_DROP, 1));  // [A > B, A2, B1, A == B]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_DROP, 1));  // [A > B, A2, A == B]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_DROP, 1));  // [A > B, A == B]
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BOR, 0));   // [A > B || A == B]
            } else {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 1));
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 1));
                REQUIRE_OK(translate_relational_greater(context->ast_context->type_traits, common_type, builder));
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 2));
                REQUIRE_OK(translate_relational_equals(common_type, builder));
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BOR, 0));
            }
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected relational or equality operation");
    }
    return KEFIR_OK;
}

static kefir_result_t truncate_value(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                     struct kefir_irbuilder_block *builder, const struct kefir_ast_type *type) {
    const struct kefir_ast_type *normalized_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, type);
    if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(normalized_type)) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDTRUNC1, 0));
    } else {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_TRUNCATE1, 0));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_logical_and(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                            struct kefir_irbuilder_block *builder,
                                            const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
    REQUIRE_OK(truncate_value(mem, context, builder, node->arg1->properties.type));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0));
    kefir_size_t jmpIndex = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BRANCH, 0));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
    REQUIRE_OK(truncate_value(mem, context, builder, node->arg2->properties.type));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BAND, 0));
    kefir_size_t jmpTarget = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);
    KEFIR_IRBUILDER_BLOCK_INSTR_AT(builder, jmpIndex)->arg.i64 = jmpTarget;
    return KEFIR_OK;
}

static kefir_result_t translate_logical_or(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                           struct kefir_irbuilder_block *builder,
                                           const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
    REQUIRE_OK(truncate_value(mem, context, builder, node->arg1->properties.type));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    kefir_size_t jmpIndex = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BRANCH, 0));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
    REQUIRE_OK(truncate_value(mem, context, builder, node->arg2->properties.type));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BOR, 0));
    kefir_size_t jmpTarget = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);
    KEFIR_IRBUILDER_BLOCK_INSTR_AT(builder, jmpIndex)->arg.i64 = jmpTarget;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_binary_operation_node(struct kefir_mem *mem,
                                                         struct kefir_ast_translator_context *context,
                                                         struct kefir_irbuilder_block *builder,
                                                         const struct kefir_ast_binary_operation *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST binary operation node"));

    switch (node->type) {
        case KEFIR_AST_OPERATION_ADD:
            REQUIRE_OK(translate_addition(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_SUBTRACT:
            REQUIRE_OK(translate_subtraction(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_MULTIPLY:
            REQUIRE_OK(translate_multiplication(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_DIVIDE:
            REQUIRE_OK(translate_division(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_MODULO:
            REQUIRE_OK(translate_modulo(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_SHIFT_RIGHT:
        case KEFIR_AST_OPERATION_SHIFT_LEFT:
        case KEFIR_AST_OPERATION_BITWISE_AND:
        case KEFIR_AST_OPERATION_BITWISE_OR:
        case KEFIR_AST_OPERATION_BITWISE_XOR:
            REQUIRE_OK(translate_bitwise(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_LESS:
        case KEFIR_AST_OPERATION_LESS_EQUAL:
        case KEFIR_AST_OPERATION_GREATER:
        case KEFIR_AST_OPERATION_GREATER_EQUAL:
        case KEFIR_AST_OPERATION_EQUAL:
        case KEFIR_AST_OPERATION_NOT_EQUAL:
            REQUIRE_OK(translate_relational_equality(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_LOGICAL_AND:
            REQUIRE_OK(translate_logical_and(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_LOGICAL_OR:
            REQUIRE_OK(translate_logical_or(mem, context, builder, node));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected AST binary operation type");
    }
    return KEFIR_OK;
}
