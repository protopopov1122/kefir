#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_case_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     const struct kefir_ast_case_statement *node,
                                                     struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST case statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;
    // TODO Check surrounding switch statement

    if (node->expression != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->expression));
        struct kefir_ast_constant_expression_value value;
        REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->expression, &value));
        REQUIRE(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected AST case label to be an integral constant expression"));
    }

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->statement));
    REQUIRE(node->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected AST statement node to be associated with the case"));
    return KEFIR_OK;
}
