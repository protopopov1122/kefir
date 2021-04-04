#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t unary_prologue(struct kefir_mem *mem,
                                   const struct kefir_ast_translator_context *context,
                                   struct kefir_irbuilder_block *builder,
                                   const struct kefir_ast_unary_operation *node) {
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->arg, builder, context));
    if (!KEFIR_AST_TYPE_SAME(node->arg->properties.type, node->base.properties.type)) {
        kefir_ast_translate_typeconv(builder, node->arg->properties.type, node->base.properties.type);
    }
    return KEFIR_OK;
}

static kefir_result_t translate_arithmetic_unary(struct kefir_mem *mem,
                                               const struct kefir_ast_translator_context *context,
                                               struct kefir_irbuilder_block *builder,
                                               const struct kefir_ast_unary_operation *node) {
    REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(node->arg->properties.type),
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-arithmetic unary AST expressions are not supported"));
    REQUIRE_OK(unary_prologue(mem, context, builder, node));
    switch (node->base.properties.type->tag) {
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            switch (node->type) {
                case KEFIR_AST_OPERATION_PLUS:
                    return KEFIR_OK;
                        
                case KEFIR_AST_OPERATION_NEGATE:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F64NEG, 0);
                
                default:
                    break;
            }
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            switch (node->type) {
                case KEFIR_AST_OPERATION_PLUS:
                    return KEFIR_OK;
                        
                case KEFIR_AST_OPERATION_NEGATE:
                    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32NEG, 0);

                default:
                    break;
            }
            break;

        default:
            if (KEFIR_AST_TYPE_IS_SIGNED_INTEGER(node->base.properties.type) ||
                KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(node->base.properties.type)) {
                switch (node->type) {
                    case KEFIR_AST_OPERATION_PLUS:
                        return KEFIR_OK;

                    case KEFIR_AST_OPERATION_NEGATE:
                        return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INEG, 0);

                    default:
                        break;
                }
            }
            break;
    }
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unsupported combination of operation and types");
}

static kefir_result_t translate_unary_inversion(struct kefir_mem *mem,
                                              const struct kefir_ast_translator_context *context,
                                              struct kefir_irbuilder_block *builder,
                                              const struct kefir_ast_unary_operation *node) {
    REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(node->arg->properties.type),
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-arithmetic unary AST expressions are not supported"));
    REQUIRE_OK(unary_prologue(mem, context, builder, node));
    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INOT, 0);
}

static kefir_result_t translate_logical_not_inversion(struct kefir_mem *mem,
                                                    const struct kefir_ast_translator_context *context,
                                                    struct kefir_irbuilder_block *builder,
                                                    const struct kefir_ast_unary_operation *node) {
    REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(node->arg->properties.type),
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-scalar unary AST expressions are not supported"));
    REQUIRE_OK(unary_prologue(mem, context, builder, node));
    return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0);
}

kefir_result_t kefir_ast_translate_unary_operation_node(struct kefir_mem *mem,
                                                    const struct kefir_ast_translator_context *context,
                                                    struct kefir_irbuilder_block *builder,
                                                    const struct kefir_ast_unary_operation *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST unary operation node"));

    switch (node->type) {
        case KEFIR_AST_OPERATION_PLUS:
        case KEFIR_AST_OPERATION_NEGATE:
            return translate_arithmetic_unary(mem, context, builder, node);

        case KEFIR_AST_OPERATION_INVERT:
            return translate_unary_inversion(mem, context, builder, node);

        case KEFIR_AST_OPERATION_LOGICAL_NEGATE:
            return translate_logical_not_inversion(mem, context, builder, node);
            
        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected AST unary operation");
    }
    return KEFIR_OK;
}