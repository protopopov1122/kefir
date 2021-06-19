#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_declaration_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                  const struct kefir_ast_declaration *node,
                                                  struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declaration"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_DECLARATION;
    REQUIRE_OK(kefir_ast_analyze_declaration(
        mem, context, &node->specifiers, node->declarator, &base->properties.declaration_props.identifier, &type,
        &storage, &base->properties.declaration_props.function, &base->properties.declaration_props.alignment));
    REQUIRE_OK(kefir_ast_analyze_type(mem, context, context->type_analysis_context, type));

    if (base->properties.declaration_props.identifier != NULL) {
        struct kefir_ast_alignment *alignment = NULL;
        if (base->properties.declaration_props.alignment != 0) {
            alignment = kefir_ast_alignment_const_expression(
                mem, kefir_ast_constant_expression_integer(mem, base->properties.declaration_props.alignment));
            REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST alignment"));
        }

        const struct kefir_ast_scoped_identifier *scoped_id = NULL;
        kefir_result_t res = context->define_identifier(
            mem, context, node->initializer == NULL, base->properties.declaration_props.identifier, type, storage,
            base->properties.declaration_props.function, alignment, node->initializer, &scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_alignment_free(mem, alignment);
            return res;
        });

        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST scoped identifier"));
        switch (scoped_id->klass) {
            case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT:
                base->properties.type = scoped_id->object.type;
                base->properties.declaration_props.storage = scoped_id->object.storage;
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
                base->properties.type = scoped_id->function.type;
                base->properties.declaration_props.storage = scoped_id->function.storage;
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION:
                base->properties.type = scoped_id->type;
                base->properties.declaration_props.storage = storage;
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:
            case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG:
            case KEFIR_AST_SCOPE_IDENTIFIER_LABEL:
                return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST scoped identifier class");
        }

        base->properties.declaration_props.scoped_id = scoped_id;
    } else {
        base->properties.type = type;
        base->properties.declaration_props.storage = storage;
    }
    return KEFIR_OK;
}
