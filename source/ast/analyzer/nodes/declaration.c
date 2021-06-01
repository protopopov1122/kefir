#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_declaration_node(struct kefir_mem *mem,
                                                  const struct kefir_ast_context *context,
                                                  const struct kefir_ast_declaration *node,
                                                  struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declaration"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_DECLARATION;
    REQUIRE_OK(kefir_ast_analyze_declaration(mem, context, &node->specifiers, node->declarator,
        &base->properties.declaration_props.identifier, &base->properties.type,
        &base->properties.declaration_props.storage,
        &base->properties.declaration_props.function,
        &base->properties.declaration_props.alignment));

    struct kefir_ast_alignment *alignment = NULL;
    if (base->properties.declaration_props.alignment != 0) {
        alignment = kefir_ast_alignment_const_expression(mem, kefir_ast_constant_expression_integer(mem,
            base->properties.declaration_props.alignment));
        REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST alignment"));
    }

    kefir_result_t res = context->define_identifier(mem, context, true /* TODO Initializer support */,
        base->properties.declaration_props.identifier,
        base->properties.type,
        base->properties.declaration_props.storage,
        base->properties.declaration_props.function,
        alignment);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_alignment_free(mem, alignment);
        return res;
    });
    return KEFIR_OK;
}
