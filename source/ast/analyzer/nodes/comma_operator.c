#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_comma_operator_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     const struct kefir_ast_comma_operator *node,
                                                     struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST comma"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE(kefir_list_length(&node->expressions) > 0,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Comma expression shall have at least one operand"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->expressions); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, expr, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, expr));
        REQUIRE(expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All comma operands shall be expressions"));

        base->properties.type = expr->properties.type;
    }

    return KEFIR_OK;
}
