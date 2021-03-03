#include <string.h>
#include "kefir/ast/context.h"
#include "kefir/ast/context_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t free_scoped_identifier(struct kefir_mem *mem,
                                           struct kefir_ast_scoped_identifier *scoped_id,
                                           void *payload) {
    UNUSED(payload);
    switch (scoped_id->klass) {
        case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT:
            REQUIRE_OK(kefir_ast_alignment_free(mem, scoped_id->object.alignment));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:
            REQUIRE_OK(kefir_ast_constant_expression_free(mem, scoped_id->enum_constant));
            break;

        default:
            break;
    }
    KEFIR_FREE(mem, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_init(struct kefir_mem *mem,
                                  struct kefir_ast_global_context *global,
                                  struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(global != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global translation context"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    context->global = global;
    REQUIRE_OK(kefir_ast_identifier_block_scope_init(mem, &context->local_ordinary_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_on_removal(&context->local_ordinary_scope, free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_block_scope_init(mem, &context->local_tag_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_on_removal(&context->local_tag_scope, free_scoped_identifier, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_free(struct kefir_mem *mem,
                                 struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_free(mem, &context->local_tag_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_free(mem, &context->local_ordinary_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_resolve_object_identifier(const struct kefir_ast_context *context,
                                                       const char *identifier,
                                                       const struct kefir_ast_scoped_identifier **scoped_identifier,
                                                       kefir_ast_scoped_identifier_linkage_t *linkage) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    struct kefir_ast_scoped_identifier *result = NULL;
    kefir_result_t res = kefir_ast_identifier_block_scope_at(&context->local_ordinary_scope, identifier, &result);
    if (res == KEFIR_NOT_FOUND) {
        res = kefir_ast_identifier_flat_scope_at(&context->global->ordinary_scope, identifier, &result);
        REQUIRE_OK(res);
        *scoped_identifier = result;
        REQUIRE((*scoped_identifier)->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Cannot found object with specified identifier"));
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
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected storage class for global variable");
            }
        }
    } else {
        REQUIRE_OK(res);
        *scoped_identifier = result;
        REQUIRE((*scoped_identifier)->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Cannot found object with specified identifier"));
        if (linkage != NULL) {
            switch ((*scoped_identifier)->object.storage) {
                case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
                case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
                    *linkage = KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE;
                    break;

                case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
                case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
                case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
                case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
                    *linkage = KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE;
                    break;
                
                case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
                        return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected storage class for local variable");
            }
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_resolve_scoped_ordinary_identifier(const struct kefir_ast_context *context,
                                                       const char *identifier,
                                                       const struct kefir_ast_scoped_identifier **scoped_identifier) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    struct kefir_ast_scoped_identifier *result = NULL;
    kefir_result_t res = kefir_ast_identifier_block_scope_at(&context->local_ordinary_scope, identifier, &result);
    if (res == KEFIR_NOT_FOUND) {
        res = kefir_ast_identifier_flat_scope_at(&context->global->ordinary_scope, identifier, &result);
    }
    if (res == KEFIR_OK) {
        *scoped_identifier = result;
    }
    return res;
}

kefir_result_t kefir_ast_context_resolve_scoped_tag_identifier(const struct kefir_ast_context *context,
                                                           const char *identifier,
                                                           const struct kefir_ast_scoped_identifier **scoped_identifier) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    struct kefir_ast_scoped_identifier *result = NULL;
    kefir_result_t res = kefir_ast_identifier_block_scope_at(&context->local_tag_scope, identifier, &result);
    if (res == KEFIR_NOT_FOUND) {
        res = kefir_ast_identifier_flat_scope_at(&context->global->tag_scope, identifier, &result);
    }
    if (res == KEFIR_OK) {
        *scoped_identifier = result;
    }
    return res;
}

kefir_result_t kefir_ast_context_push_block_scope(struct kefir_mem *mem,
                                              struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_push(mem, &context->local_ordinary_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_push(mem, &context->local_tag_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_pop_block_scope(struct kefir_ast_context *context) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_pop(&context->local_tag_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_pop(&context->local_ordinary_scope));
    return KEFIR_OK;
}

static struct kefir_ast_scoped_identifier *allocate_scoped_object_identifier(struct kefir_mem *mem,
                                                                           const struct kefir_ast_type *type,
                                                                           kefir_ast_scoped_identifier_storage_t storage,
                                                                           struct kefir_ast_alignment *alignment) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_OBJECT;
    scoped_id->object.type = type;
    scoped_id->object.storage = storage;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    if (alignment != NULL) {
        scoped_id->object.alignment = alignment;
    } else {
        scoped_id->object.alignment = kefir_ast_alignment_default(mem);
        REQUIRE_ELSE(scoped_id->object.alignment != NULL, {
            KEFIR_FREE(mem, scoped_id);
            return NULL;
        });
    }
    return scoped_id;
}

static struct kefir_ast_scoped_identifier *allocate_scoped_constant(struct kefir_mem *mem,
                                                                  struct kefir_ast_constant_expression *value) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT;
    scoped_id->enum_constant = value;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    return scoped_id;
}

static struct kefir_ast_scoped_identifier *allocate_scoped_type_tag(struct kefir_mem *mem,
                                                                  const struct kefir_ast_type *type) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG;
    scoped_id->type = type;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    return scoped_id;
}

static struct kefir_ast_scoped_identifier *allocate_scoped_type_definition(struct kefir_mem *mem,
                                                                         const struct kefir_ast_type *type) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION;
    scoped_id->type = type;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    return scoped_id;
}

kefir_result_t kefir_ast_context_declare_external(struct kefir_mem *mem,
                                              struct kefir_ast_context *context,
                                              const char *identifier,
                                              const struct kefir_ast_type *type,
                                              struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    bool external_declaration_exists = false;
    if (kefir_hashtree_has(&context->global->external_object_declarations, (kefir_hashtree_key_t) identifier)) {
        struct kefir_hashtree_node *external_node;
        REQUIRE_OK(kefir_hashtree_at(&context->global->external_object_declarations, (kefir_hashtree_key_t) identifier, &external_node));
        scoped_id = (struct kefir_ast_scoped_identifier *) external_node->value;
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        external_declaration_exists = true;
    }

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->local_ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->global->type_bundle, context->global->type_traits,
            scoped_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
            struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_ordinary_scope, id, scoped_id));
        if (!external_declaration_exists) {
            REQUIRE_OK(kefir_hashtree_insert(mem, &context->global->external_object_declarations,
                (kefir_hashtree_key_t) id, (kefir_hashtree_value_t) scoped_id));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_declare_external_thread_local(struct kefir_mem *mem,
                                                           struct kefir_ast_context *context,
                                                           const char *identifier,
                                                           const struct kefir_ast_type *type,
                                                           struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    bool external_declaration_exists = false;
    if (kefir_hashtree_has(&context->global->external_object_declarations, (kefir_hashtree_key_t) identifier)) {
        struct kefir_hashtree_node *external_node;
        REQUIRE_OK(kefir_hashtree_at(&context->global->external_object_declarations, (kefir_hashtree_key_t) identifier, &external_node));
        scoped_id = (struct kefir_ast_scoped_identifier *) external_node->value;
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        external_declaration_exists = true;
    }

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->local_ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->global->type_bundle, context->global->type_traits,
            scoped_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
            struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_ordinary_scope, id, scoped_id));
        if (!external_declaration_exists) {
            REQUIRE_OK(kefir_hashtree_insert(mem, &context->global->external_object_declarations,
                (kefir_hashtree_key_t) id, (kefir_hashtree_value_t) scoped_id));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_static(struct kefir_mem *mem,
                                           struct kefir_ast_context *context,
                                           const char *identifier,
                                           const struct kefir_ast_type *type,
                                           struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->local_ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_static_thread_local(struct kefir_mem *mem,
                                                          struct kefir_ast_context *context,
                                                          const char *identifier,
                                                          const struct kefir_ast_type *type,
                                                          struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->local_ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_auto(struct kefir_mem *mem,
                                         struct kefir_ast_context *context,
                                         const char *identifier,
                                         const struct kefir_ast_type *type,
                                         struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
        
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->local_ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_register(struct kefir_mem *mem,
                                             struct kefir_ast_context *context,
                                             const char *identifier,
                                             const struct kefir_ast_type *type,
                                             struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
            kefir_ast_identifier_block_scope_top(&context->local_ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
            struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_constant(struct kefir_mem *mem,
                                             struct kefir_ast_context *context,
                                             const char *identifier,
                                             struct kefir_ast_constant_expression *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(kefir_ast_identifier_block_scope_top(&context->local_ordinary_scope),
        identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine constant");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_constant(mem, value);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_tag(struct kefir_mem *mem,
                                        struct kefir_ast_context *context,
                                        const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const char *identifier = NULL;
    REQUIRE_OK(kefir_ast_context_type_retrieve_tag(type, &identifier));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(kefir_ast_identifier_block_scope_top(&context->local_tag_scope),
        identifier, (struct kefir_ast_scoped_identifier **) &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE_OK(kefir_ast_context_update_existing_scoped_type_tag(scoped_id, type));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            allocate_scoped_type_tag(mem, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_tag_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_define_type(struct kefir_mem *mem,
                                         struct kefir_ast_context *context,
                                         const char *identifier,
                                         const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(kefir_ast_identifier_block_scope_top(&context->local_ordinary_scope),
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
            allocate_scoped_type_definition(mem, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}