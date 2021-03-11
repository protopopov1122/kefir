#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_unary_operation_node(struct kefir_mem *mem,
                                                  const struct kefir_ast_context *context,
                                                  const struct kefir_ast_unary_operation *node,
                                                  struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST unary operation"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->arg));
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    switch (node->type) {
        case KEFIR_AST_OPERATION_PLUS:
        case KEFIR_AST_OPERATION_NEGATE: {
            const struct kefir_ast_type *type1 = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->arg->properties.type);
            REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected arithmetic argument of unary +|-"));
            if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1)) {
                base->properties.type = kefir_ast_type_int_promotion(context->type_traits, type1);
            } else {
                base->properties.type = type1;
            }
            base->properties.expression_props.constant_expression =
                node->arg->properties.expression_props.constant_expression;
        } break;
        
        case KEFIR_AST_OPERATION_INVERT: {
            const struct kefir_ast_type *type1 = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->arg->properties.type);
            REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integral argument of bitwise inversion"));
            base->properties.type = kefir_ast_type_int_promotion(context->type_traits, type1);
            base->properties.expression_props.constant_expression =
                node->arg->properties.expression_props.constant_expression;
        } break;

        case KEFIR_AST_OPERATION_LOGICAL_NEGATE: {
            const struct kefir_ast_type *type1 = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->arg->properties.type);
            REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(type1),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected type argument of logical negation"));
            base->properties.type = kefir_ast_type_signed_int();
            base->properties.expression_props.constant_expression =
                node->arg->properties.expression_props.constant_expression;
        } break;

        case KEFIR_AST_OPERATION_PREFIX_INCREMENT:
        case KEFIR_AST_OPERATION_PREFIX_DECREMENT:
        case KEFIR_AST_OPERATION_POSTFIX_INCREMENT:
        case KEFIR_AST_OPERATION_POSTFIX_DECREMENT: {
            REQUIRE(node->arg->properties.expression_props.lvalue,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected lvalue argument"));
            const struct kefir_ast_type *type = node->arg->properties.type;
            REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(type),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected scalar type argument"));
            base->properties.type = type;
        } break;

        case KEFIR_AST_OPERATION_ADDRESS: {
            REQUIRE(node->arg->properties.expression_props.addressable,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected addressable argument"));
            const struct kefir_ast_type *type = node->arg->properties.type;
            base->properties.type = kefir_ast_type_pointer(mem, context->type_bundle, type);
        } break;

        case KEFIR_AST_OPERATION_INDIRECTION: {
            const struct kefir_ast_type *type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem,
                context->type_bundle, node->arg->properties.type);
            REQUIRE(type->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected type argument of logical negation"));
            type = type->referenced_type;
            base->properties.type = type;
            if (type->tag != KEFIR_AST_TYPE_FUNCTION) {
                base->properties.expression_props.lvalue = true;
            }
            base->properties.expression_props.addressable = true;
        } break;

        // TODO: Implement sizeof and alignof

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected unary AST node type");
    }
    return KEFIR_OK;
}