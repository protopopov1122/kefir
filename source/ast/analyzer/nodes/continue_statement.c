#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t resolve_loop(const struct kefir_ast_flow_control_statement *stmt, void *payload,
                                   kefir_bool_t *result) {
    UNUSED(payload);
    REQUIRE(stmt != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control statement"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to boolean"));

    switch (stmt->type) {
        case KEFIR_AST_FLOW_CONTROL_STATEMENT_IF:
        case KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH:
            *result = false;
            break;

        case KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR:
        case KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE:
        case KEFIR_AST_FLOW_CONTROL_STATEMENT_DO:
            *result = true;
            break;
    }

    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_continue_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                         const struct kefir_ast_continue_statement *node,
                                                         struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST continue statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    REQUIRE(context->flow_control_tree != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Continue statement is not allowed in current context"));

    struct kefir_ast_flow_control_statement *flow_control_stmt = NULL;
    REQUIRE_OK(
        kefir_ast_flow_control_tree_traverse(context->flow_control_tree, resolve_loop, NULL, &flow_control_stmt));
    base->properties.statement_props.flow_control_point = flow_control_stmt->value.loop.begin;
    base->properties.statement_props.flow_control_statement = flow_control_stmt;
    return KEFIR_OK;
}
