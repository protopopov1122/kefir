#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_binary_operation_node(struct kefir_mem *mem,
                                                   const struct kefir_ast_context *context,
                                                   const struct kefir_ast_binary_operation *node,
                                                   struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST binary operation"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));
    
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->arg1));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->arg2));
    const struct kefir_ast_type *type1 = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle,
        node->arg1->properties.type);
    const struct kefir_ast_type *type2 = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle,
        node->arg2->properties.type);
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.expression_props.constant_expression =
        node->arg1->properties.expression_props.constant_expression &&
        node->arg2->properties.expression_props.constant_expression;
    switch (node->type) {
        case KEFIR_AST_OPERATION_SHIFT_LEFT:
        case KEFIR_AST_OPERATION_SHIFT_RIGHT: {
            REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1) &&
                KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type2),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Bitwise shift operator expects integer arguments"));
            base->properties.type = kefir_ast_type_int_promotion(context->type_traits, type1);
            REQUIRE(base->properties.type != NULL,
                KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Unable to determine common AST arithmetic type"));
        } break;

        default: {
            if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1) &&
                KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type2)) {
                base->properties.type = kefir_ast_type_common_arithmetic(context->type_traits, type1, type2);
                REQUIRE(base->properties.type != NULL,
                    KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR,
                        "Unable to determine common AST arithmetic type"));
            } else {
                return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED,
                    "Binary type derival from non-arithmetic AST types is not supported yet");
            }
        }
    }
    return KEFIR_OK;
}