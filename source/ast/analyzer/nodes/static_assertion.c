#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_static_assertion_node(struct kefir_mem *mem,
                                            const struct kefir_ast_context *context,
                                            const struct kefir_ast_static_assertion *node,
                                            struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST static assertion"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATIC_ASSERTION;
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->condition));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(node->string)));
    
    struct kefir_ast_constant_expression_value value;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->condition, &value));
    REQUIRE(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Static assertion condition shall be an integral constant expression"));
    REQUIRE(value.integer != 0, KEFIR_SET_ERROR(KEFIR_STATIC_ASSERT, "Static assertion failure")); // TODO Provide string from assertion
    return KEFIR_OK;
}
