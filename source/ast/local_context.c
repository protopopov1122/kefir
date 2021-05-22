#include <string.h>
#include <stdio.h>
#include "kefir/ast/local_context.h"
#include "kefir/ast/context_impl.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/initializer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t free_scoped_identifier(struct kefir_mem *mem,
                                           struct kefir_list *list,
                                           struct kefir_list_entry *entry,
                                           void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid scoped identifier list entry"));
    UNUSED(list);
    ASSIGN_DECL_CAST(struct kefir_ast_scoped_identifier *, scoped_id,
        entry->value);
    return kefir_ast_context_free_scoped_identifier(mem, scoped_id, payload);
}

static kefir_result_t context_resolve_ordinary_identifier(const struct kefir_ast_context *context,
                                                        const char *identifier,
                                                        const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx,
        context->payload);
    return kefir_ast_local_context_resolve_scoped_ordinary_identifier(local_ctx, identifier, scoped_id);
}

static kefir_result_t context_resolve_tag_identifier(const struct kefir_ast_context *context,
                                                   const char *identifier,
                                                   const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx,
        context->payload);
    return kefir_ast_local_context_resolve_scoped_tag_identifier(local_ctx, identifier, scoped_id);
}

static kefir_result_t context_allocate_temporary_value(struct kefir_mem *mem,
                                                     const struct kefir_ast_context *context,
                                                     const struct kefir_ast_type *type,
                                                     struct kefir_ast_temporary_identifier *temp_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(temp_id != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid temporary identifier pointer"));

    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx,
        context->payload);
    if (kefir_ast_temporaries_init(mem, context->type_bundle, context->temporaries)) {
        REQUIRE(context->temporaries->type != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to initialize a temporary type"));
        REQUIRE_OK(kefir_ast_local_context_define_auto(mem, local_ctx,
            KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER, context->temporaries->type, NULL, NULL));
    }
    REQUIRE_OK(kefir_ast_temporaries_new_temporary(mem, context, type, temp_id));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_init(struct kefir_mem *mem,
                                  struct kefir_ast_global_context *global,
                                  struct kefir_ast_local_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(global != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global translation context"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    context->global = global;
    context->temporaries = (struct kefir_ast_context_temporaries){0};
    REQUIRE_OK(kefir_list_init(&context->identifiers));
    REQUIRE_OK(kefir_list_on_remove(&context->identifiers, free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_block_scope_init(mem, &context->ordinary_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_init(mem, &context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_on_removal(&context->tag_scope, kefir_ast_context_free_scoped_identifier, NULL));

    context->context.resolve_ordinary_identifier = context_resolve_ordinary_identifier;
    context->context.resolve_tag_identifier = context_resolve_tag_identifier;
    context->context.allocate_temporary_value = context_allocate_temporary_value;
    context->context.symbols = &context->global->symbols;
    context->context.type_bundle = &context->global->type_bundle;
    context->context.type_traits = context->global->type_traits;
    context->context.target_env = context->global->target_env;
    context->context.temporaries = &context->temporaries;
    context->context.payload = context;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_free(struct kefir_mem *mem,
                                 struct kefir_ast_local_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_free(mem, &context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_free(mem, &context->ordinary_scope));
    REQUIRE_OK(kefir_list_free(mem, &context->identifiers));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_resolve_scoped_ordinary_identifier(const struct kefir_ast_local_context *context,
                                                       const char *identifier,
                                                       const struct kefir_ast_scoped_identifier **scoped_identifier) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    struct kefir_ast_scoped_identifier *result = NULL;
    kefir_result_t res = kefir_ast_identifier_block_scope_at(&context->ordinary_scope, identifier, &result);
    if (res == KEFIR_NOT_FOUND) {
        res = kefir_ast_global_context_resolve_scoped_ordinary_identifier(context->global, identifier, scoped_identifier);
    } else if (res == KEFIR_OK) {
        *scoped_identifier = result;
    }
    return res;
}

kefir_result_t kefir_ast_local_context_resolve_scoped_tag_identifier(const struct kefir_ast_local_context *context,
                                                           const char *identifier,
                                                           const struct kefir_ast_scoped_identifier **scoped_identifier) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    struct kefir_ast_scoped_identifier *result = NULL;
    kefir_result_t res = kefir_ast_identifier_block_scope_at(&context->tag_scope, identifier, &result);
    if (res == KEFIR_NOT_FOUND) {
        res = kefir_ast_global_context_resolve_scoped_tag_identifier(context->global, identifier, scoped_identifier);
    } else if (res == KEFIR_OK) {
        *scoped_identifier = result;
    }
    return res;
}

kefir_result_t kefir_ast_local_context_push_block_scope(struct kefir_mem *mem,
                                              struct kefir_ast_local_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_push(mem, &context->ordinary_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_push(mem, &context->tag_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_pop_block_scope(struct kefir_ast_local_context *context) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_pop(&context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_pop(&context->ordinary_scope));
    return KEFIR_OK;
}

static kefir_result_t require_global_ordinary_object(struct kefir_ast_global_context *context,
                                                   const char *identifier,
                                                   kefir_bool_t thread_local,
                                                   const struct kefir_ast_type *type,
                                                   struct kefir_ast_scoped_identifier **ordinary_id) {
    *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_identifiers, identifier, ordinary_id);
    if (res == KEFIR_OK) {
        if (thread_local) {
            REQUIRE((*ordinary_id)->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL ||
                (*ordinary_id)->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redeclare identifier with different storage class"));
        } else {
            REQUIRE((*ordinary_id)->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
                (*ordinary_id)->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redeclare identifier with different storage class"));
        }
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, (*ordinary_id)->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redeclare identifier with incompatible types"));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_declare_external(struct kefir_mem *mem,
                                              struct kefir_ast_local_context *context,
                                              const char *identifier,
                                              const struct kefir_ast_type *type,
                                              struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    identifier = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    struct kefir_ast_scoped_identifier *global_ordinary_id = NULL, *ordinary_id = NULL;
    REQUIRE_OK(require_global_ordinary_object(context->global, identifier, false, type, &global_ordinary_id));

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(ordinary_id == global_ordinary_id,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Local extern object identifier cannot be different than global"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, ordinary_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->global->type_bundle, context->global->type_traits,
            ordinary_id->object.type, type);
    } else if (global_ordinary_id != NULL) {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, global_ordinary_id));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &global_ordinary_id->object.alignment, alignment));
        global_ordinary_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->global->type_bundle, context->global->type_traits,
            global_ordinary_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *ordinary_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                alignment, KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE, true, NULL);
        REQUIRE(ordinary_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->global->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, ordinary_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_declare_external_thread_local(struct kefir_mem *mem,
                                                           struct kefir_ast_local_context *context,
                                                           const char *identifier,
                                                           const struct kefir_ast_type *type,
                                                           struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    identifier = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    struct kefir_ast_scoped_identifier *global_ordinary_id = NULL, *ordinary_id = NULL;
    REQUIRE_OK(require_global_ordinary_object(context->global, identifier, true, type, &global_ordinary_id));

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(ordinary_id == global_ordinary_id,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Local extern object identifier cannot be different than global"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, ordinary_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->global->type_bundle, context->global->type_traits,
            ordinary_id->object.type, type);
    } else if (global_ordinary_id != NULL) {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, global_ordinary_id));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &global_ordinary_id->object.alignment, alignment));
        global_ordinary_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->global->type_bundle, context->global->type_traits,
            global_ordinary_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *ordinary_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                alignment, KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE, true, NULL);
        REQUIRE(ordinary_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->global->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, ordinary_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_static(struct kefir_mem *mem,
                                           struct kefir_ast_local_context *context,
                                           const char *identifier,
                                           const struct kefir_ast_type *type,
                                           struct kefir_ast_alignment *alignment,
                                           struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    if (initializer != NULL) {
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;
    }
    
    REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Identifier with no linkage shall have complete type"));
    
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, alignment,
                KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE, false, initializer);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_static_thread_local(struct kefir_mem *mem,
                                                          struct kefir_ast_local_context *context,
                                                          const char *identifier,
                                                          const struct kefir_ast_type *type,
                                                          struct kefir_ast_alignment *alignment,
                                                          struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    
    if (initializer != NULL) {
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;
    }
    
    REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Identifier with no linkage shall have complete type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                alignment, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE, false, initializer);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_auto(struct kefir_mem *mem,
                                         struct kefir_ast_local_context *context,
                                         const char *identifier,
                                         const struct kefir_ast_type *type,
                                         struct kefir_ast_alignment *alignment,
                                         struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    if (initializer != NULL) {
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;
    }
    
    REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Identifier with no linkage shall have complete type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, alignment,
                KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE, false, initializer);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_register(struct kefir_mem *mem,
                                             struct kefir_ast_local_context *context,
                                             const char *identifier,
                                             const struct kefir_ast_type *type,
                                             struct kefir_ast_alignment *alignment,
                                             struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    
    if (initializer != NULL) {
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;
    }
    
    REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Identifier with no linkage shall have complete type"));
        
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, alignment,
                KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE, false, initializer);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_constant(struct kefir_mem *mem,
                                             struct kefir_ast_local_context *context,
                                             const char *identifier,
                                             struct kefir_ast_constant_expression *value,
                                             const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(kefir_ast_identifier_block_scope_top(&context->ordinary_scope),
        identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine constant");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_constant(mem, value, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_tag(struct kefir_mem *mem,
                                        struct kefir_ast_local_context *context,
                                        const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const char *identifier = NULL;
    REQUIRE_OK(kefir_ast_context_type_retrieve_tag(type, &identifier));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(kefir_ast_identifier_block_scope_top(&context->tag_scope),
        identifier, (struct kefir_ast_scoped_identifier **) &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE_OK(kefir_ast_context_update_existing_scoped_type_tag(scoped_id, type));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_type_tag(mem, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        res = kefir_ast_identifier_block_scope_insert(mem, &context->tag_scope, id, scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_type(struct kefir_mem *mem,
                                         struct kefir_ast_local_context *context,
                                         const char *identifier,
                                         const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(kefir_ast_identifier_block_scope_top(&context->ordinary_scope),
        identifier, (struct kefir_ast_scoped_identifier **) &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to redefine identifier with different kind of symbol"));
        REQUIRE(KEFIR_AST_TYPE_SAME(type, scoped_id->type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to redefine different type with the same identifier"));
        return KEFIR_OK;
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_type_definition(mem, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

static kefir_result_t require_global_ordinary_function(struct kefir_ast_global_context *context,
                                                     const char *identifier,
                                                     const struct kefir_ast_type *type,
                                                     struct kefir_ast_scoped_identifier **ordinary_id) {
    *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->function_identifiers, identifier, ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, (*ordinary_id)->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redeclare identifier with incompatible types"));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_declare_function(struct kefir_mem *mem,
                                                    struct kefir_ast_local_context *context,
                                                    kefir_ast_function_specifier_t specifier,
                                                    const struct kefir_ast_type *function) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(function != NULL && function->tag == KEFIR_AST_TYPE_FUNCTION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(function->function_type.identifier != NULL && strlen(function->function_type.identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected function with non-empty identifier"));

    const char *identifier = function->function_type.identifier;

    struct kefir_ast_scoped_identifier *global_ordinary_id = NULL, *ordinary_id = NULL;
    REQUIRE_OK(require_global_ordinary_function(context->global, identifier, function, &global_ordinary_id));

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(ordinary_id == global_ordinary_id,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Local extern function identifier cannot be different than global"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, ordinary_id->function.type, function),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        ordinary_id->function.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->global->type_bundle, context->global->type_traits,
            ordinary_id->function.type, function);
        ordinary_id->function.specifier = kefir_ast_context_merge_function_specifiers(ordinary_id->function.specifier, specifier);
    } else if (global_ordinary_id != NULL) {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, global_ordinary_id));
        global_ordinary_id->function.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->global->type_bundle, context->global->type_traits,
            global_ordinary_id->function.type, function);
        global_ordinary_id->function.specifier = kefir_ast_context_merge_function_specifiers(global_ordinary_id->function.specifier, specifier);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *ordinary_id =
            kefir_ast_context_allocate_scoped_function_identifier(mem, function, specifier,
                KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, true);
        REQUIRE(ordinary_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->global->function_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, ordinary_id));
    }
    return KEFIR_OK;
}
