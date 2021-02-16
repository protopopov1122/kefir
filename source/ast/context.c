#include <string.h>
#include "kefir/ast/context.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t free_scoped_identifier(struct kefir_mem *mem,
                                           struct kefir_ast_scoped_identifier *scoped_id,
                                           void *payload) {
    UNUSED(payload);
    KEFIR_FREE(mem, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_init(struct kefir_mem *mem,
                                                     const struct kefir_ast_type_traits *type_traits,
                                                     struct kefir_ast_global_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_traits != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type traits"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_symbol_table_init(&context->symbols));
    REQUIRE_OK(kefir_ast_type_repository_init(&context->type_repository, &context->symbols));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->object_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->object_scope, free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_hashtree_init(&context->external_object_declarations, &kefir_hashtree_str_ops));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_free(struct kefir_mem *mem,
                                                     struct kefir_ast_global_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_hashtree_free(mem, &context->external_object_declarations));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->object_scope));
    REQUIRE_OK(kefir_ast_type_repository_free(mem, &context->type_repository));
    REQUIRE_OK(kefir_symbol_table_free(mem, &context->symbols));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_init(struct kefir_mem *mem,
                                              struct kefir_ast_global_context *global,
                                              struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(global != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global translation context"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    context->global = global;
    REQUIRE_OK(kefir_ast_identifier_block_scope_init(mem, &context->local_object_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_on_removal(&context->local_object_scope, free_scoped_identifier, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_free(struct kefir_mem *mem,
                                              struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_free(mem, &context->local_object_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_resolve_object_identifier(const struct kefir_ast_context *context,
                                                       const char *identifier,
                                                       const struct kefir_ast_scoped_identifier **scoped_identifier,
                                                       kefir_ast_scoped_identifier_linkage_t *linkage) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    kefir_result_t res = kefir_ast_identifier_block_scope_at(&context->local_object_scope, identifier, scoped_identifier);
    if (res == KEFIR_NOT_FOUND) {
        res = kefir_ast_identifier_flat_scope_at(&context->global->object_scope, identifier, scoped_identifier);
    }
    REQUIRE_OK(res);
    if (linkage != NULL) {
        switch ((*scoped_identifier)->object.storage) {
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
                *linkage = KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE;
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
                *linkage = KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE;
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
                *linkage = KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE;
                break;
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_push_block_scope(struct kefir_mem *mem,
                                                          struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_push(mem, &context->local_object_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_pop_block_scope(struct kefir_ast_context *context) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_pop(&context->local_object_scope));
    return KEFIR_OK;
}

static struct kefir_ast_scoped_identifier *allocate_scoped_object_identifier(struct kefir_mem *mem,
                                                                           const struct kefir_ast_type *type,
                                                                           kefir_ast_scoped_identifier_storage_t storage) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_OBJECT;
    scoped_id->object.type = type;
    scoped_id->object.storage = storage;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    return scoped_id;
}

kefir_result_t kefir_ast_global_context_declare_external(struct kefir_mem *mem,
                                                     struct kefir_ast_global_context *context,
                                                     const char *identifier,
                                                     const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
            scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
    } else {
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->object_scope, id, scoped_id));
        REQUIRE_OK(kefir_hashtree_insert(mem, &context->external_object_declarations,
            (kefir_hashtree_key_t) id, (kefir_hashtree_value_t) scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_declare_external_thread_local(struct kefir_mem *mem,
                                                                  struct kefir_ast_global_context *context,
                                                                  const char *identifier,
                                                                  const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL ||
            scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
    } else {
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->object_scope, id, scoped_id));
        REQUIRE_OK(kefir_hashtree_insert(mem, &context->external_object_declarations,
            (kefir_hashtree_key_t) id, (kefir_hashtree_value_t) scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_external(struct kefir_mem *mem,
                                                    struct kefir_ast_global_context *context,
                                                    const char *identifier,
                                                    const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        if (kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier)) {
            REQUIRE_OK(kefir_hashtree_delete(mem, &context->external_object_declarations, (kefir_hashtree_key_t) identifier));
        }
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->object_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_external_thread_local(struct kefir_mem *mem,
                                                                 struct kefir_ast_global_context *context,
                                                                 const char *identifier,
                                                                 const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        if (kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier)) {
            REQUIRE_OK(kefir_hashtree_delete(mem, &context->external_object_declarations, (kefir_hashtree_key_t) identifier));
        }
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->object_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_static(struct kefir_mem *mem,
                                                  struct kefir_ast_global_context *context,
                                                  const char *identifier,
                                                  const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        REQUIRE(!kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier),
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Identifier with static storage duration cannot be external"));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->object_scope, id, scoped_id));
    }
    return KEFIR_OK;   
}

kefir_result_t kefir_ast_global_context_define_static_thread_local(struct kefir_mem *mem,
                                                               struct kefir_ast_global_context *context,
                                                               const char *identifier,
                                                               const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        REQUIRE(!kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier),
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Identifier with static storage duration cannot be external"));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->object_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_declare_external(struct kefir_mem *mem,
                                              struct kefir_ast_context *context,
                                              const char *identifier,
                                              const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    struct kefir_ast_scoped_identifier *scoped_id =
        allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
    // TODO: Check existence and type compatibility
    const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_object_scope, id, scoped_id));
    REQUIRE_OK(kefir_hashtree_insert(mem, &context->global->external_object_declarations,
        (kefir_hashtree_key_t) id, (kefir_hashtree_value_t) scoped_id));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_declare_external_thread_local(struct kefir_mem *mem,
                                                           struct kefir_ast_context *context,
                                                           const char *identifier,
                                                           const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    struct kefir_ast_scoped_identifier *scoped_id =
        allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
    // TODO: Check existence and type compatibility
    const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_object_scope, id, scoped_id));
    REQUIRE_OK(kefir_hashtree_insert(mem, &context->global->external_object_declarations,
        (kefir_hashtree_key_t) id, (kefir_hashtree_value_t) scoped_id));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_static(struct kefir_mem *mem,
                                           struct kefir_ast_context *context,
                                           const char *identifier,
                                           const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    struct kefir_ast_scoped_identifier *scoped_id =
        allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
    // TODO: Check existence and type compatibility
    const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_object_scope, id, scoped_id));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_external_thread_local(struct kefir_mem *mem,
                                                          struct kefir_ast_context *context,
                                                          const char *identifier,
                                                          const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    struct kefir_ast_scoped_identifier *scoped_id =
        allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
    REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
    // TODO: Check existence and type compatibility
    const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_object_scope, id, scoped_id));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_auto(struct kefir_mem *mem,
                                         struct kefir_ast_context *context,
                                         const char *identifier,
                                         const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    struct kefir_ast_scoped_identifier *scoped_id =
        allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
    REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
    // TODO: Check existence and type compatibility
    const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_object_scope, id, scoped_id));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_register(struct kefir_mem *mem,
                                             struct kefir_ast_context *context,
                                             const char *identifier,
                                             const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    struct kefir_ast_scoped_identifier *scoped_id =
        allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER);
    REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
    // TODO: Check existence and type compatibility
    const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_object_scope, id, scoped_id));
    return KEFIR_OK;
}