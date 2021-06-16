#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_goto_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     const struct kefir_ast_goto_statement *node,
                                                     struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST goto statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE_OK(context->reference_label(mem, context, node->identifier, false, &scoped_id));
    REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Goto statement identifier should reference a label"));
    base->properties.statement_props.flow_control_point = scoped_id->label.point;
    return KEFIR_OK;
}
