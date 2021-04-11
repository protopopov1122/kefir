#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t translate_arithmetic_unary(struct kefir_mem *mem,
                                               struct kefir_ast_translator_context *context,
                                               struct kefir_irbuilder_block *builder,
                                               const struct kefir_ast_unary_operation *node) {
    const struct kefir_ast_type *normalized_type = kefir_ast_translator_normalize_type(node->arg->properties.type);
    REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(normalized_type),
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-arithmetic unary AST expressions are not supported"));

    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg, builder, context));
    switch (normalized_type->tag) {
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
            REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(normalized_type),
                 KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integral type as operand of unary arithmetic operator"));
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

static kefir_result_t translate_unary_inversion(struct kefir_mem *mem,
                                              struct kefir_ast_translator_context *context,
                                              struct kefir_irbuilder_block *builder,
                                              const struct kefir_ast_unary_operation *node) {
    const struct kefir_ast_type *normalized_type = kefir_ast_translator_normalize_type(node->arg->properties.type);
    REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(normalized_type),
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-arithmetic unary AST expressions are not supported"));
        
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg, builder, context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INOT, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_logical_not_inversion(struct kefir_mem *mem,
                                                    struct kefir_ast_translator_context *context,
                                                    struct kefir_irbuilder_block *builder,
                                                    const struct kefir_ast_unary_operation *node) {
    const struct kefir_ast_type *normalized_type = kefir_ast_translator_normalize_type(node->arg->properties.type);
    REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(normalized_type),
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-scalar unary AST expressions are not supported"));
    
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg, builder, context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_sizeof(struct kefir_mem *mem,
                                     struct kefir_ast_translator_context *context,
                                     struct kefir_irbuilder_block *builder,
                                     const struct kefir_ast_unary_operation *node) {                                        
    kefir_ast_target_environment_opaque_type_t opaque_type;
    struct kefir_ast_target_environment_object_info type_info;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, &context->environment->target_env,
        node->arg->properties.type, &opaque_type));
    kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, &context->environment->target_env,
        opaque_type, NULL, &type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type);
        return res;
    });
    
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, type_info.size));
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type));
    return KEFIR_OK;
}


static kefir_result_t translate_alignof(struct kefir_mem *mem,
                                      struct kefir_ast_translator_context *context,
                                      struct kefir_irbuilder_block *builder,
                                      const struct kefir_ast_unary_operation *node) {                                        
    kefir_ast_target_environment_opaque_type_t opaque_type;
    struct kefir_ast_target_environment_object_info type_info;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, &context->environment->target_env,
        node->arg->properties.type, &opaque_type));
    kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, &context->environment->target_env,
        opaque_type, NULL, &type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type);
        return res;
    });
    
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, type_info.alignment));
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_unary_operation_node(struct kefir_mem *mem,
                                                    struct kefir_ast_translator_context *context,
                                                    struct kefir_irbuilder_block *builder,
                                                    const struct kefir_ast_unary_operation *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST unary operation node"));

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

        case KEFIR_AST_OPERATION_POSTFIX_INCREMENT:
        case KEFIR_AST_OPERATION_POSTFIX_DECREMENT:
        case KEFIR_AST_OPERATION_PREFIX_INCREMENT:
        case KEFIR_AST_OPERATION_PREFIX_DECREMENT:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Increment/decrement operation translation is not implemented yet");

        case KEFIR_AST_OPERATION_SIZEOF:
            REQUIRE_OK(translate_sizeof(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_ALIGNOF:
            REQUIRE_OK(translate_alignof(mem, context, builder, node));
            break;

        case KEFIR_AST_OPERATION_ADDRESS:
        case KEFIR_AST_OPERATION_INDIRECTION:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Not implemented yet");
            
        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected AST unary operation");
    }
    return KEFIR_OK;
}