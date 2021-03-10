#include "kefir/ast/analyzer/nodes.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_identifier_node(struct kefir_mem *mem,
                                             const struct kefir_ast_context *context,
                                             const struct kefir_ast_identifier *node,
                                             struct kefir_ast_node_base *base) {
    UNUSED(mem);
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(
        context, node->identifier, &scoped_id));
    switch (scoped_id->klass) {
        case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT:
            REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
            base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
            base->properties.type = scoped_id->object.type;
            base->properties.expression_props.lvalue = true;
            return KEFIR_OK;

        case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
            REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
            base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
            base->properties.type = scoped_id->function.type;
            return KEFIR_OK;

        case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:
            REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
            base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
            base->properties.type = scoped_id->enum_constant.type;
            base->properties.expression_props.constant_expression = true;
            return KEFIR_OK;
        
        default:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-object identifiers are not supported yet");
    }
}