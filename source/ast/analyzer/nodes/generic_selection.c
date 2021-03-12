#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_generic_selection_node(struct kefir_mem *mem,
                                                    const struct kefir_ast_context *context,
                                                    const struct kefir_ast_generic_selection *node,
                                                    struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST generic selection"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->control));
    const struct kefir_ast_type *control_type = node->control->properties.type;
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->associations);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc,
            iter->value);
        if (KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, control_type, assoc->type)) {
            REQUIRE_OK(kefir_ast_analyze_node(mem, context, assoc->expr));
            REQUIRE_OK(kefir_ast_node_properties_clone(&base->properties, &assoc->expr->properties));
            return KEFIR_OK;
        }
    }
    if (node->default_assoc != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->default_assoc));
        REQUIRE_OK(kefir_ast_node_properties_clone(&base->properties, &node->default_assoc->properties));
        return KEFIR_OK;
    } else {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected at least one of associations in generic selection to be compatible"
            " with control expression type");
    }
}