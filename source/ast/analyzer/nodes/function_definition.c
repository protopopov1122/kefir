#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_function_definition_node(struct kefir_mem *mem,
                                                          const struct kefir_ast_context *context,
                                                          const struct kefir_ast_function_definition *node,
                                                          struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function definition"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION;

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    kefir_size_t alignment = 0;
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_DECLARATION;
    REQUIRE_OK(kefir_ast_analyze_declaration(mem, context, &node->specifiers, node->declarator,
                                             &base->properties.function_definition.identifier, &type, &storage,
                                             &base->properties.function_definition.function, &alignment));
    REQUIRE_OK(kefir_ast_analyze_type(mem, context, context->type_analysis_context, type));

    REQUIRE(type->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function definition declarator shall have function type"));
    REQUIRE(base->properties.function_definition.identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function definition shall have non-empty identifier"));
    REQUIRE(alignment == 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function definition cannot have non-zero alignment"));
    switch (storage) {
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Invalid function definition storage specifier");

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
            // Intentionally left blank
            break;
    }

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE_OK(context->define_identifier(mem, context, NULL, base->properties.function_definition.identifier, type,
                                          storage, base->properties.function_definition.function, NULL, NULL,
                                          &scoped_id));
    REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Scoped identifier does not correspond to function definition type"));

    base->properties.type = scoped_id->function.type;
    base->properties.function_definition.storage = scoped_id->function.storage;
    base->properties.declaration_props.scoped_id = scoped_id;

    // TODO Analyze declaration list and set up local function context

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(node->body)));

    return KEFIR_OK;
}
