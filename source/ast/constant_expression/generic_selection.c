#include "kefir/ast/constant_expression_impl.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_evaluate_generic_selection_node(struct kefir_mem *mem,
                                                     const struct kefir_ast_context *context,
                                                     const struct kefir_ast_generic_selection *node,
                                                     struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST string literalnode"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION &&
        node->base.properties.expression_props.constant_expression,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected constant expression AST node"));
    
    const struct kefir_ast_type *control_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle,
        node->control->properties.type);
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->associations);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc,
            iter->value);
        if (KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, control_type, assoc->type)) {
            return kefir_ast_constant_expression_evaluate(mem, context, assoc->expr, value);
        }
    }
    if (node->default_assoc != NULL) {
        return kefir_ast_constant_expression_evaluate(mem, context, node->default_assoc, value);
    }

    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected at least one of associations in generic selection to be compatible"
            " with control expression type");
}