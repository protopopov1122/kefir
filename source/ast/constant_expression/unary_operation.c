#include "kefir/ast/constant_expression_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_evaluate_unary_operation_node(struct kefir_mem *mem,
                                                   const struct kefir_ast_context *context,
                                                   const struct kefir_ast_unary_operation *node,
                                                   struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST unary operation node"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION &&
        node->base.properties.expression_props.constant_expression,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected constant expression AST node"));

    struct kefir_ast_constant_expression_value arg_value;
    REQUIRE_OK(kefir_ast_constant_expression_evaluate(mem, context, node->arg, &arg_value));
    switch (node->type) {
            case KEFIR_AST_OPERATION_PLUS:
                if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                    value->integer = arg_value.integer;
                } else if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                    value->floating_point = arg_value.floating_point;
                } else {
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integeral or floating-point constant expression");
                }
                break;
                
            case KEFIR_AST_OPERATION_NEGATE:
                if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                    value->integer = -arg_value.integer;
                } else if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                    value->floating_point = -arg_value.floating_point;
                } else {
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integeral or floating-point constant expression");
                }
                break;
                
            case KEFIR_AST_OPERATION_INVERT:
                if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                    value->integer = ~arg_value.integer;
                } else {
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integeral constant expression");
                }
                break;
                
            case KEFIR_AST_OPERATION_LOGICAL_NEGATE:
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                    value->integer = !arg_value.integer;
                } else if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                    value->integer = !arg_value.floating_point;
                } else {
                    value->integer = true;
                }
                break;
                
            case KEFIR_AST_OPERATION_POSTFIX_INCREMENT:
            case KEFIR_AST_OPERATION_POSTFIX_DECREMENT:
            case KEFIR_AST_OPERATION_PREFIX_INCREMENT:
            case KEFIR_AST_OPERATION_PREFIX_DECREMENT:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                    "Constant expressions shall not contain increment/decrement operators");
                
            case KEFIR_AST_OPERATION_ADDRESS:
                return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED,
                    "Addressed constant expressions are not implemented yet");
                
            case KEFIR_AST_OPERATION_INDIRECTION:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                    "Constant expression cannot contain indirection operator");
                
            case KEFIR_AST_OPERATION_SIZEOF:
            case KEFIR_AST_OPERATION_ALIGNOF:
                return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED,
                    "Sizeof/alignof constant expressions are not implemented yet");
    }
    return KEFIR_OK;
}