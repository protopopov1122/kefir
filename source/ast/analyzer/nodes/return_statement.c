#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_return_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_return_statement *node,
                                                       struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST return statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    if (node->expression != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->expression));
        REQUIRE(node->expression->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Return statement should return an expression"));
    }
    return KEFIR_OK;
}
