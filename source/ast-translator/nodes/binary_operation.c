#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t binary_prologue(struct kefir_mem *mem,
                                    struct kefir_ast_translator_context *context,
                                    struct kefir_irbuilder_block *builder,
                                    const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
    REQUIRE_OK(kefir_ast_translate_typeconv(builder, node->arg1->properties.type, node->base.properties.type));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
    REQUIRE_OK(kefir_ast_translate_typeconv(builder, node->arg2->properties.type, node->base.properties.type));
    return KEFIR_OK;
}

static kefir_result_t translate_addition(struct kefir_mem *mem,
                                       struct kefir_ast_translator_context *context,
                                       struct kefir_irbuilder_block *builder,
                                       const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *arg1_normalized_type = kefir_ast_translator_normalize_type(node->arg1->properties.type);
    const struct kefir_ast_type *arg2_normalized_type = kefir_ast_translator_normalize_type(node->arg2->properties.type);
    const struct kefir_ast_type *result_normalized_type = kefir_ast_translator_normalize_type(node->base.properties.type);

    if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg1_normalized_type) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg2_normalized_type)) {
        REQUIRE_OK(binary_prologue(mem, context, builder, node));
        switch (result_normalized_type->tag) {
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
        const struct kefir_ast_translator_cached_type *cached_type = NULL;
        REQUIRE_OK(kefir_ast_translator_type_cache_generate_owned(mem, node->base.properties.type->referenced_type, 0,
            &context->type_cache, context->environment, context->module, &cached_type));

        if (arg1_normalized_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
            REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
            REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
        } else {
            REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
            REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
        }
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_ELEMENTPTR, cached_type->ir_type_id, 0));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_subtraction(struct kefir_mem *mem,
                                          struct kefir_ast_translator_context *context,
                                          struct kefir_irbuilder_block *builder,
                                          const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *arg1_normalized_type = kefir_ast_translator_normalize_type(node->arg1->properties.type);
    const struct kefir_ast_type *arg2_normalized_type = kefir_ast_translator_normalize_type(node->arg2->properties.type);
    const struct kefir_ast_type *result_normalized_type = kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg1_normalized_type) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(arg2_normalized_type)) {
        switch (result_normalized_type->tag) {
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
        const struct kefir_ast_translator_cached_type *cached_type = NULL;
        REQUIRE_OK(kefir_ast_translator_type_cache_generate_owned(mem, arg1_normalized_type->referenced_type, 0,
            &context->type_cache, context->environment, context->module, &cached_type));

        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INEG, 0));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_ELEMENTPTR, cached_type->ir_type_id, 0));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_multiplication(struct kefir_mem *mem,
                                             struct kefir_ast_translator_context *context,
                                             struct kefir_irbuilder_block *builder,
                                             const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *result_normalized_type = kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    switch (result_normalized_type->tag) {
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

static kefir_result_t translate_division(struct kefir_mem *mem,
                                       struct kefir_ast_translator_context *context,
                                       struct kefir_irbuilder_block *builder,
                                       const struct kefir_ast_binary_operation *node) {
    const struct kefir_ast_type *result_normalized_type = kefir_ast_translator_normalize_type(node->base.properties.type);

    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    switch (result_normalized_type->tag) {
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

static kefir_result_t translate_modulo(struct kefir_mem *mem,
                                     struct kefir_ast_translator_context *context,
                                     struct kefir_irbuilder_block *builder,
                                     const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IMOD, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_bitwise_left_shift(struct kefir_mem *mem,
                                                 struct kefir_ast_translator_context *context,
                                                 struct kefir_irbuilder_block *builder,
                                                 const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ILSHIFT, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_bitwise_right_shift(struct kefir_mem *mem,
                                                  struct kefir_ast_translator_context *context,
                                                  struct kefir_irbuilder_block *builder,
                                                  const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IARSHIFT, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_bitwise_and(struct kefir_mem *mem,
                                          struct kefir_ast_translator_context *context,
                                          struct kefir_irbuilder_block *builder,
                                          const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IAND, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_bitwise_or(struct kefir_mem *mem,
                                         struct kefir_ast_translator_context *context,
                                         struct kefir_irbuilder_block *builder,
                                         const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IOR, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_bitwise_xor(struct kefir_mem *mem,
                                          struct kefir_ast_translator_context *context,
                                          struct kefir_irbuilder_block *builder,
                                          const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IXOR, 0));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_binary_operation_node(struct kefir_mem *mem,
                                                     struct kefir_ast_translator_context *context,
                                                     struct kefir_irbuilder_block *builder,
                                                     const struct kefir_ast_binary_operation *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST binary operation node"));
        
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
            REQUIRE_OK(translate_bitwise_right_shift(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_SHIFT_LEFT:
            REQUIRE_OK(translate_bitwise_left_shift(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_BITWISE_AND:
            REQUIRE_OK(translate_bitwise_and(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_BITWISE_OR:
            REQUIRE_OK(translate_bitwise_or(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_BITWISE_XOR:
            REQUIRE_OK(translate_bitwise_xor(mem, context, builder, node));
            break;
        
        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected AST binary operation type");
    }
    return KEFIR_OK;
}