#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/local_context.h"
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
    base->properties.function_definition.scoped_id = scoped_id;

    struct kefir_ast_local_context *local_context = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_local_context));
    REQUIRE(local_context != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST local context"));

    kefir_result_t res = kefir_ast_local_context_init(mem, context->global_context, local_context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, local_context);
        return res;
    });
    *scoped_id->function.local_context_ptr = local_context;

    switch (scoped_id->function.type->function_type.mode) {
        case KEFIR_AST_FUNCTION_TYPE_PARAMETERS:
            REQUIRE(kefir_list_length(&node->declarations) == 0,
                    KEFIR_SET_ERROR(
                        KEFIR_MALFORMED_ARG,
                        "Function definition with non-empty parameter list shall not contain any declarations"));

            for (const struct kefir_list_entry *iter =
                     kefir_list_head(&scoped_id->function.type->function_type.parameters);
                 iter != NULL; kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, param, iter->value);
                REQUIRE(
                    param->type != NULL,
                    KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Function definition parameters shall have definite types"));

                if (type->tag != KEFIR_AST_TYPE_VOID) {
                    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, local_context, param->identifier, param->type,
                                                                   NULL, NULL, NULL));
                }
            }
            break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS:
            for (const struct kefir_list_entry *iter = kefir_list_head(&node->declarations); iter != NULL;
                 kefir_list_next(&iter)) {

                ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl_node, iter->value);
                REQUIRE(decl_node->klass->type == KEFIR_AST_DECLARATION,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Function definition declaration list shall contain exclusively declarations"));
                ASSIGN_DECL_CAST(struct kefir_ast_declaration *, decl, decl_node->self);

                const char *identifier = NULL;
                const struct kefir_ast_type *type = NULL;
                kefir_ast_scoped_identifier_storage_t storage = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
                kefir_ast_function_specifier_t function_specifier = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
                kefir_size_t alignment = 0;
                REQUIRE_OK(kefir_ast_analyze_declaration(mem, &local_context->context, &decl->specifiers,
                                                         decl->declarator, &identifier, &type, &storage,
                                                         &function_specifier, &alignment));

                REQUIRE(kefir_hashtree_has(&scoped_id->function.type->function_type.parameter_index,
                                           (kefir_hashtree_key_t) identifier),
                        KEFIR_SET_ERROR(
                            KEFIR_MALFORMED_ARG,
                            "Function definition declaration list declarations shall refer to identifier list"));
                REQUIRE(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN ||
                            storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function definition declaration list shall not contain "
                                                             "storage class specifiers other than register"));
                REQUIRE(alignment == 0,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Function definition declaration list shall not contain alignment specifiers"));

                const struct kefir_ast_scoped_identifier *param_scoped_id = NULL;
                REQUIRE_OK(local_context->context.define_identifier(mem, &local_context->context, false, identifier,
                                                                    type, storage, function_specifier, NULL, NULL,
                                                                    &param_scoped_id));
                decl->base.properties.declaration_props.scoped_id = param_scoped_id;
            }

            for (const struct kefir_list_entry *iter =
                     kefir_list_head(&scoped_id->function.type->function_type.parameters);
                 iter != NULL; kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, param, iter->value);
                REQUIRE(param->type == NULL,
                        KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                        "Function definition identifiers shall not have definite types"));

                const struct kefir_ast_scoped_identifier *scoped_id = NULL;
                REQUIRE_OK(local_context->context.resolve_ordinary_identifier(&local_context->context,
                                                                              param->identifier, &scoped_id));
            }
            break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY:
            REQUIRE(
                kefir_list_length(&node->declarations) == 0,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Function definition with empty parameter list shall not contain any declarations"));
            break;
    }

    REQUIRE_OK(kefir_ast_node_properties_init(&node->body->base.properties));
    node->body->base.properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->body->block_items); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, &local_context->context, item));
        REQUIRE(item->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT ||
                    item->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Compound statement items shall be either statements or declarations"));
    }

    return KEFIR_OK;
}
