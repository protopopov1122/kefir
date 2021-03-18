#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_array_subscript_node(struct kefir_mem *mem,
                                                  const struct kefir_ast_context *context,
                                                  const struct kefir_ast_array_subscript *node,
                                                  struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST array subscript"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->array));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->subscript));

    REQUIRE(node->array->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both array subscript operands shall be expressions"));
    REQUIRE(node->subscript->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Both array subscript operands shall be expressions"));

    const struct kefir_ast_type *array_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem,
        context->type_bundle, node->array->properties.type);
    const struct kefir_ast_type *subcript_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem,
        context->type_bundle, node->subscript->properties.type);
    const struct kefir_ast_type *type = NULL;
    if (array_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(subcript_type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected one of expressions to have integral type"));
        type = array_type->referenced_type;
    } else {
        REQUIRE(subcript_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected one of expressions to have pointer type"));
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(array_type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected one of expressions to have integral type"));
        type = subcript_type->referenced_type;
    }
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type = type;
    base->properties.expression_props.lvalue = true;
    base->properties.expression_props.addressable = true;
    return KEFIR_OK;
}