#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_conditional_statement_node(struct kefir_mem *mem,
                                                            const struct kefir_ast_context *context,
                                                            const struct kefir_ast_conditional_statement *node,
                                                            struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST conditional statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    REQUIRE(context->flow_control_tree != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to use conditional statement in current context"));
    REQUIRE_OK(kefir_ast_flow_control_tree_push(mem, context->flow_control_tree, KEFIR_AST_FLOW_CONTROL_STATEMENT_IF,
                                                &base->properties.statement_props.flow_control_statement));

    REQUIRE_OK(context->push_block(mem, context));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->condition));
    REQUIRE(node->condition->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected if statement condition to be scalar expression"));
    REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(node->condition->properties.type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected if statement condition to be scalar expression"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->thenBranch));
    REQUIRE(node->thenBranch->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected the first if branch to be a statement"));

    if (node->elseBranch != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->elseBranch));
        REQUIRE(node->elseBranch->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected the second if branch to be a statement"));
    }

    REQUIRE_OK(context->pop_block(mem, context));
    REQUIRE_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    return KEFIR_OK;
}
