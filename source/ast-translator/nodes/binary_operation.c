#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t binary_prologue(struct kefir_mem *mem,
                                    struct kefir_ast_translator_context *context,
                                    struct kefir_irbuilder_block *builder,
                                    const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg1, builder, context));
    if (!KEFIR_AST_TYPE_SAME(node->arg1->properties.type, node->base.properties.type)) {
        kefir_ast_translate_typeconv(builder, node->arg1->properties.type, node->base.properties.type);
    }
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg2, builder, context));
    if (!KEFIR_AST_TYPE_SAME(node->arg2->properties.type, node->base.properties.type)) {
        kefir_ast_translate_typeconv(builder, node->arg2->properties.type, node->base.properties.type);
    }
    return KEFIR_OK;
}

static kefir_result_t translate_addition(struct kefir_mem *mem,
                                       struct kefir_ast_translator_context *context,
                                       struct kefir_irbuilder_block *builder,
                                       const struct kefir_ast_binary_operation *node) {
    REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(node->arg1->properties.type) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(node->arg2->properties.type),
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-arithmetic additive AST expressions are not supported yet"));
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    switch (node->base.properties.type->tag) {
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            switch (node->type) {
                case KEFIR_AST_OPERATION_ADD:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64ADD, 0);

                case KEFIR_AST_OPERATION_SUBTRACT:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64SUB, 0);
                
                default:
                    break;
            }
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            switch (node->type) {
                case KEFIR_AST_OPERATION_ADD:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32ADD, 0);

                case KEFIR_AST_OPERATION_SUBTRACT:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32SUB, 0);

                default:
                    break;
            }
            break;

        default:
            if (KEFIR_AST_TYPE_IS_SIGNED_INTEGER(node->base.properties.type) ||
                KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(node->base.properties.type)) {
                switch (node->type) {
                    case KEFIR_AST_OPERATION_ADD:
                        return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IADD, 0);

                    case KEFIR_AST_OPERATION_SUBTRACT:
                        return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ISUB, 0);

                    default:
                        break;
                }
            }
            break;
    }
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unsupported combination of operation and types");
}

static kefir_result_t translate_multiplication(struct kefir_mem *mem,
                                             struct kefir_ast_translator_context *context,
                                             struct kefir_irbuilder_block *builder,
                                             const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    switch (node->base.properties.type->tag) {
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            switch (node->type) {
                case KEFIR_AST_OPERATION_MULTIPLY:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64MUL, 0);

                case KEFIR_AST_OPERATION_DIVIDE:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64DIV, 0);

                default:
                    break;
            }
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            switch (node->type) {
                case KEFIR_AST_OPERATION_MULTIPLY:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32MUL, 0);

                case KEFIR_AST_OPERATION_DIVIDE:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32DIV, 0);

                default:
                    break;
            }
            break;

        default:
            if (KEFIR_AST_TYPE_IS_SIGNED_INTEGER(node->base.properties.type) ||
                KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(node->base.properties.type)) {
                switch (node->type) {
                    case KEFIR_AST_OPERATION_MULTIPLY:
                        return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IMUL, 0);

                    case KEFIR_AST_OPERATION_DIVIDE:
                        return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IDIV, 0);

                    case KEFIR_AST_OPERATION_MODULO:
                        return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IMOD, 0);

                    default:
                        break;
                }
            }
            break;
    }
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unsupported combination of operation and types");
}

static kefir_result_t translate_bitwise_shift(struct kefir_mem *mem,
                                            struct kefir_ast_translator_context *context,
                                            struct kefir_irbuilder_block *builder,
                                            const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(mem, context, builder, node));
    if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(node->base.properties.type)) {
        switch (node->type) {
            case KEFIR_AST_OPERATION_SHIFT_LEFT:
                return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ILSHIFT, 0);

            case KEFIR_AST_OPERATION_SHIFT_RIGHT:
                return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IARSHIFT, 0);

            default:
                break;
        }
    }
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unsupported combination of operation and types");
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
        case KEFIR_AST_OPERATION_SUBTRACT:
            return translate_addition(mem, context, builder, node);

        case KEFIR_AST_OPERATION_MULTIPLY:
        case KEFIR_AST_OPERATION_DIVIDE:
        case KEFIR_AST_OPERATION_MODULO:
            return translate_multiplication(mem, context, builder, node);

        case KEFIR_AST_OPERATION_SHIFT_RIGHT:
        case KEFIR_AST_OPERATION_SHIFT_LEFT:
            return translate_bitwise_shift(mem, context, builder, node);
        
        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected AST binary operation type");
    }
    return KEFIR_OK;
}