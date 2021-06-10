#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_string_literal_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     const struct kefir_ast_string_literal *node,
                                                     struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST string literal"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type = kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_char(),
                                                 kefir_ast_constant_expression_integer(mem, node->length), NULL);
    base->properties.expression_props.constant_expression = true;
    base->properties.expression_props.string_literal.content = node->literal;
    base->properties.expression_props.string_literal.length = node->length;
    return KEFIR_OK;
}
