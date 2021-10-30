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
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast-translator/value.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

static kefir_result_t translate_arithmetic_unary(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                 struct kefir_irbuilder_block *builder,
                                                 const struct kefir_ast_unary_operation *node) {
    const struct kefir_ast_type *normalized_type = kefir_ast_translator_normalize_type(node->arg->properties.type);
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg, builder, context));
    switch (normalized_type->tag) {
        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
            switch (node->type) {
                case KEFIR_AST_OPERATION_PLUS:
                    break;

                case KEFIR_AST_OPERATION_NEGATE:
                    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDNEG, 0));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected unary operator");
            }
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            switch (node->type) {
                case KEFIR_AST_OPERATION_PLUS:
                    break;

                case KEFIR_AST_OPERATION_NEGATE:
                    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64NEG, 0));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected unary operator");
            }
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            switch (node->type) {
                case KEFIR_AST_OPERATION_PLUS:
                    break;

                case KEFIR_AST_OPERATION_NEGATE:
                    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32NEG, 0));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected unary operator");
            }
            break;

        default:
            REQUIRE(
                KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(normalized_type),
                KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected integral type as operand of unary arithmetic operator"));
            switch (node->type) {
                case KEFIR_AST_OPERATION_PLUS:
                    break;

                case KEFIR_AST_OPERATION_NEGATE:
                    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INEG, 0));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected unary operator");
            }
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t translate_unary_inversion(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                struct kefir_irbuilder_block *builder,
                                                const struct kefir_ast_unary_operation *node) {
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg, builder, context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INOT, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_logical_not_inversion(struct kefir_mem *mem,
                                                      struct kefir_ast_translator_context *context,
                                                      struct kefir_irbuilder_block *builder,
                                                      const struct kefir_ast_unary_operation *node) {
    const struct kefir_ast_type *normalized_type = kefir_ast_translator_normalize_type(node->arg->properties.type);
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg, builder, context));
    if (normalized_type->tag == KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDTRUNC1, 0));
    }
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_sizeof(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                       struct kefir_irbuilder_block *builder,
                                       const struct kefir_ast_unary_operation *node) {
    kefir_ast_target_environment_opaque_type_t opaque_type;
    struct kefir_ast_target_environment_object_info type_info;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, &context->environment->target_env, node->arg->properties.type,
                                                     &opaque_type));
    kefir_result_t res =
        KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, &context->environment->target_env, opaque_type, NULL, &type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type);
        return res;
    });

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, type_info.size));
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type));
    return KEFIR_OK;
}

static kefir_result_t translate_alignof(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                        struct kefir_irbuilder_block *builder,
                                        const struct kefir_ast_unary_operation *node) {
    kefir_ast_target_environment_opaque_type_t opaque_type;
    struct kefir_ast_target_environment_object_info type_info;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, &context->environment->target_env, node->arg->properties.type,
                                                     &opaque_type));
    kefir_result_t res =
        KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, &context->environment->target_env, opaque_type, NULL, &type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type);
        return res;
    });

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, type_info.alignment));
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type));
    return KEFIR_OK;
}

static kefir_result_t translate_indirection(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                            struct kefir_irbuilder_block *builder,
                                            const struct kefir_ast_unary_operation *node) {
    const struct kefir_ast_type *normalized_type = kefir_ast_translator_normalize_type(node->base.properties.type);
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg, builder, context));
    REQUIRE_OK(kefir_ast_translator_load_value(normalized_type, context->ast_context->type_traits, builder));
    return KEFIR_OK;
}

static kefir_result_t incdec_impl(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                  struct kefir_irbuilder_block *builder, const struct kefir_ast_unary_operation *node,
                                  const struct kefir_ast_type *normalized_type) {
    kefir_int64_t diff =
        node->type == KEFIR_AST_OPERATION_POSTFIX_INCREMENT || node->type == KEFIR_AST_OPERATION_PREFIX_INCREMENT ? 1
                                                                                                                  : -1;
    switch (normalized_type->tag) {
        case KEFIR_AST_TYPE_SCALAR_POINTER: {
            const struct kefir_ast_translator_resolved_type *cached_type = NULL;
            REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(
                mem, &context->type_cache.resolver, context->environment, context->module,
                node->base.properties.type->referenced_type, 0, &cached_type));
            REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
                    KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected cached type to be an object"));

            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64, diff));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(
                builder, KEFIR_IROPCODE_ELEMENTPTR, cached_type->object.ir_type_id, cached_type->object.layout->value));
        } break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDF32(builder, KEFIR_IROPCODE_PUSHF32, (kefir_float32_t) diff, 0.0f));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32ADD, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDF64(builder, KEFIR_IROPCODE_PUSHF64, (kefir_float64_t) diff));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64ADD, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHLD,
                                                       kefir_ir_long_double_upper_half((kefir_long_double_t) diff)));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64,
                                                       kefir_ir_long_double_lower_half((kefir_long_double_t) diff)));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDADD, 0));
            break;

        default:
            REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(normalized_type),
                    KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected value of an integral type"));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IADD1, diff));
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t translate_preincdec(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                          struct kefir_irbuilder_block *builder,
                                          const struct kefir_ast_unary_operation *node) {
    const struct kefir_ast_type *normalized_type = kefir_ast_translator_normalize_type(node->base.properties.type);
    REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->arg));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(kefir_ast_translator_load_value(normalized_type, context->ast_context->type_traits, builder));

    REQUIRE_OK(incdec_impl(mem, context, builder, node, normalized_type));

    if (normalized_type->tag == KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 2));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_DROP, 3));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 2));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 2));
    } else {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 1));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 1));
    }
    REQUIRE_OK(kefir_ast_translator_store_value(mem, normalized_type, context, builder));
    return KEFIR_OK;
}

static kefir_result_t translate_postincdec(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                           struct kefir_irbuilder_block *builder,
                                           const struct kefir_ast_unary_operation *node) {
    const struct kefir_ast_type *normalized_type = kefir_ast_translator_normalize_type(node->base.properties.type);
    REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->arg));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(kefir_ast_translator_load_value(normalized_type, context->ast_context->type_traits, builder));

    if (normalized_type->tag == KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 2));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_DROP, 3));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 2));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 2));
    } else {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 1));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 1));
    }

    REQUIRE_OK(incdec_impl(mem, context, builder, node, normalized_type));
    REQUIRE_OK(kefir_ast_translator_store_value(mem, normalized_type, context, builder));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_unary_operation_node(struct kefir_mem *mem,
                                                        struct kefir_ast_translator_context *context,
                                                        struct kefir_irbuilder_block *builder,
                                                        const struct kefir_ast_unary_operation *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST unary operation node"));

    switch (node->type) {
        case KEFIR_AST_OPERATION_PLUS:
        case KEFIR_AST_OPERATION_NEGATE:
            REQUIRE_OK(translate_arithmetic_unary(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_INVERT:
            REQUIRE_OK(translate_unary_inversion(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_LOGICAL_NEGATE:
            REQUIRE_OK(translate_logical_not_inversion(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_PREFIX_INCREMENT:
        case KEFIR_AST_OPERATION_PREFIX_DECREMENT:
            REQUIRE_OK(translate_preincdec(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_POSTFIX_INCREMENT:
        case KEFIR_AST_OPERATION_POSTFIX_DECREMENT:
            REQUIRE_OK(translate_postincdec(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_SIZEOF:
            REQUIRE_OK(translate_sizeof(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_ALIGNOF:
            REQUIRE_OK(translate_alignof(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_ADDRESS:
            REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->arg));
            break;

        case KEFIR_AST_OPERATION_INDIRECTION:
            REQUIRE_OK(translate_indirection(mem, context, builder, node));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected AST unary operation");
    }
    return KEFIR_OK;
}
