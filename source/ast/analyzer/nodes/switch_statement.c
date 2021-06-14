#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_switch_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_switch_statement *node,
                                                       struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST switch statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    REQUIRE(context->flow_control_tree != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to use switch statement in current context"));
    REQUIRE_OK(context->push_block(mem, context));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->expression));
    REQUIRE(
        node->expression->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected switch controlling expression to be an integral expression"));
    REQUIRE(
        KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(node->expression->properties.type),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected switch controlling expression to be an integral expression"));
    const struct kefir_ast_type *controlling_expr_type =
        kefir_ast_type_int_promotion(context->type_traits, node->expression->properties.type);
    REQUIRE(
        controlling_expr_type != NULL,
        KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Unable to perform integral promotion on controlling expression type"));

    struct kefir_ast_flow_control_statement *stmt = NULL;
    REQUIRE_OK(kefir_ast_flow_control_tree_push(mem, context->flow_control_tree,
                                                KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt));
    stmt->value.switchStatement.controlling_expression_type = controlling_expr_type;
    base->properties.statement_props.flow_control_statement = stmt;

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->statement));
    REQUIRE(node->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected switch body to be a statement"));

    REQUIRE_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    REQUIRE_OK(context->pop_block(mem, context));
    return KEFIR_OK;
}
