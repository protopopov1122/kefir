#include "kefir/ast/translator/translator.h"
#include "kefir/ast/translator/typeconv.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"
#include "kefir/ir/builder.h"

kefir_result_t translate_not_impl(const struct kefir_ast_visitor *visitor,
                                const struct kefir_ast_node_base *base,
                                void *payload) {
    UNUSED(visitor);
    UNUSED(base);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot translate non-expression AST node");
}

kefir_result_t translate_constant(const struct kefir_ast_visitor *visitor,
                                const struct kefir_ast_constant *node,
                                void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_irbuilder_block *, builder,
        payload);
    switch (node->type) {
        case KEFIR_AST_BOOL_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, (kefir_uint64_t) node->value.boolean);

        case KEFIR_AST_CHAR_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, (kefir_uint64_t) node->value.character);

        case KEFIR_AST_INT_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHU64, (kefir_int64_t) node->value.integer);

        case KEFIR_AST_UINT_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, (kefir_uint64_t) node->value.uinteger);

        case KEFIR_AST_LONG_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHU64, (kefir_int64_t) node->value.long_integer);

        case KEFIR_AST_ULONG_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, (kefir_uint64_t) node->value.ulong_integer);

        case KEFIR_AST_LONG_LONG_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHU64, (kefir_int64_t) node->value.long_long);

        case KEFIR_AST_ULONG_LONG_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, (kefir_uint64_t) node->value.ulong_long);

        case KEFIR_AST_FLOAT_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDF32(builder, KEFIR_IROPCODE_PUSHF32, node->value.float32, 0.0f);

        case KEFIR_AST_DOUBLE_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDF64(builder, KEFIR_IROPCODE_PUSHF64, node->value.float64);


        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected AST constant type");
    }
}

kefir_result_t translate_unary(const struct kefir_ast_visitor *visitor,
                             const struct kefir_ast_unary_operation *node,
                             void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_irbuilder_block *, builder,
        payload);
    REQUIRE_OK(KEFIR_AST_NODE_VISIT(visitor, node->arg, builder));
    switch (node->type) {
        case KEFIR_AST_OPERATION_NEGATE:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INEG, 0);

        case KEFIR_AST_OPERATION_INVERT:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INOT, 0);
            
        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected AST unary operation");
    }
}

static kefir_result_t binary_prologue(const struct kefir_ast_visitor *visitor,
                                    struct kefir_irbuilder_block *builder,
                                    const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(KEFIR_AST_NODE_VISIT(visitor, node->arg1, builder));
    if (!KEFIR_AST_TYPE_SAME(node->arg1->expression_type, node->base.expression_type)) {
        kefir_ast_translate_typeconv(builder, node->arg1->expression_type, node->base.expression_type);
    }
    REQUIRE_OK(KEFIR_AST_NODE_VISIT(visitor, node->arg2, builder));
    if (!KEFIR_AST_TYPE_SAME(node->arg2->expression_type, node->base.expression_type)) {
        kefir_ast_translate_typeconv(builder, node->arg2->expression_type, node->base.expression_type);
    }
    return KEFIR_OK;
}

static kefir_result_t translate_addition(const struct kefir_ast_visitor *visitor,
                                       struct kefir_irbuilder_block *builder,
                                       const struct kefir_ast_binary_operation *node) {
    REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(node->arg1->expression_type) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(node->arg2->expression_type),
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-arithmetic additive AST expressions are not supported yet"));
    REQUIRE_OK(binary_prologue(visitor, builder, node));
    switch (node->base.expression_type->tag) {
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
            if (KEFIR_AST_TYPE_IS_SIGNED_INTEGER(node->base.expression_type) ||
                KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(node->base.expression_type)) {
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

static kefir_result_t translate_multiplication(const struct kefir_ast_visitor *visitor,
                                             struct kefir_irbuilder_block *builder,
                                             const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(visitor, builder, node));
    switch (node->base.expression_type->tag) {
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
            if (KEFIR_AST_TYPE_IS_SIGNED_INTEGER(node->base.expression_type) ||
                KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(node->base.expression_type)) {
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

static kefir_result_t translate_bitwise_shift(const struct kefir_ast_visitor *visitor,
                                            struct kefir_irbuilder_block *builder,
                                            const struct kefir_ast_binary_operation *node) {
    REQUIRE_OK(binary_prologue(visitor, builder, node));
    if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(node->base.expression_type)) {
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

kefir_result_t translate_binary(const struct kefir_ast_visitor *visitor,
                              const struct kefir_ast_binary_operation *node,
                              void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_irbuilder_block *, builder,
        payload);
    switch (node->type) {
        case KEFIR_AST_OPERATION_ADD:
        case KEFIR_AST_OPERATION_SUBTRACT:
            return translate_addition(visitor, builder, node);

        case KEFIR_AST_OPERATION_MULTIPLY:
        case KEFIR_AST_OPERATION_DIVIDE:
        case KEFIR_AST_OPERATION_MODULO:
            return translate_multiplication(visitor, builder, node);

        case KEFIR_AST_OPERATION_SHIFT_RIGHT:
        case KEFIR_AST_OPERATION_SHIFT_LEFT:
            return translate_bitwise_shift(visitor, builder, node);
        
        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected AST binary operation type");
    }
}

kefir_result_t kefir_ast_translate_expression(const struct kefir_ast_node_base *base,
                                          struct kefir_irbuilder_block *builder) {
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, translate_not_impl));
    visitor.constant = translate_constant;
    visitor.unary_operation = translate_unary;
    visitor.binary_operation = translate_binary;
    return KEFIR_AST_NODE_VISIT(&visitor, base, builder);
}