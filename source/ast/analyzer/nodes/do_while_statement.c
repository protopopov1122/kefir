#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_do_while_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                         const struct kefir_ast_do_while_statement *node,
                                                         struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST do while statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    REQUIRE(context->flow_control_tree != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to use do while statement in current context"));
    REQUIRE_OK(kefir_ast_flow_control_tree_push(mem, context->flow_control_tree, KEFIR_AST_FLOW_CONTROL_STATEMENT_DO,
                                                &base->properties.statement_props.flow_control_statement));

    base->properties.statement_props.flow_control_statement->value.loop.continuation =
        kefir_ast_flow_control_point_alloc(mem);
    REQUIRE(base->properties.statement_props.flow_control_statement->value.loop.continuation != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST flow control point"));

    base->properties.statement_props.flow_control_statement->value.loop.end = kefir_ast_flow_control_point_alloc(mem);
    REQUIRE(base->properties.statement_props.flow_control_statement->value.loop.end != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST flow control point"));

    REQUIRE_OK(context->push_block(mem, context));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->controlling_expr));
    REQUIRE(node->controlling_expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                            "Expected do while statement controlling expression to be scalar expression"));
    REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(node->controlling_expr->properties.type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected do while statement condition to be scalar expression"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->body));
    REQUIRE(node->body->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected do while statement body to be a statement"));

    REQUIRE_OK(context->pop_block(mem, context));
    REQUIRE_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    return KEFIR_OK;
}
