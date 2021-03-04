#include <string.h>
#include "kefir/ast/global_context.h"
#include "kefir/ast/context_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_global_context_init(struct kefir_mem *mem,
                                         const struct kefir_ast_type_traits *type_traits,
                                         struct kefir_ast_global_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_traits != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type traits"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_symbol_table_init(&context->symbols));
    REQUIRE_OK(kefir_ast_type_bundle_init(&context->type_bundle, &context->symbols));
    context->type_traits = type_traits;
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->ordinary_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->ordinary_scope, kefir_ast_context_free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->tag_scope, kefir_ast_context_free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_hashtree_init(&context->external_object_declarations, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_init(&context->external_function_declarations, &kefir_hashtree_str_ops));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_free(struct kefir_mem *mem,
                                         struct kefir_ast_global_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_hashtree_free(mem, &context->external_function_declarations));
    REQUIRE_OK(kefir_hashtree_free(mem, &context->external_object_declarations));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->ordinary_scope));
    REQUIRE_OK(kefir_ast_type_bundle_free(mem, &context->type_bundle));
    REQUIRE_OK(kefir_symbol_table_free(mem, &context->symbols));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_declare_external(struct kefir_mem *mem,
                                                     struct kefir_ast_global_context *context,
                                                     const char *identifier,
                                                     const struct kefir_ast_type *type,
                                                     struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    bool external_declaration_exists = false;
    if (kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier)) {
        struct kefir_hashtree_node *external_node;
        REQUIRE_OK(kefir_hashtree_at(&context->external_object_declarations, (kefir_hashtree_key_t) identifier, &external_node));
        scoped_id = (struct kefir_ast_scoped_identifier *) external_node->value;
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        external_declaration_exists = true;
    }

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
            scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
            scoped_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
        if (!external_declaration_exists) {
            REQUIRE_OK(kefir_hashtree_insert(mem, &context->external_object_declarations,
                (kefir_hashtree_key_t) id, (kefir_hashtree_value_t) scoped_id));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_declare_external_thread_local(struct kefir_mem *mem,
                                                                  struct kefir_ast_global_context *context,
                                                                  const char *identifier,
                                                                  const struct kefir_ast_type *type,
                                                                  struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    bool external_declaration_exists = false;
    if (kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier)) {
        struct kefir_hashtree_node *external_node;
        REQUIRE_OK(kefir_hashtree_at(&context->external_object_declarations, (kefir_hashtree_key_t) identifier, &external_node));
        scoped_id = (struct kefir_ast_scoped_identifier *) external_node->value;
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        external_declaration_exists = true;
    }

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL ||
            scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
            scoped_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
        if (!external_declaration_exists) {
            REQUIRE_OK(kefir_hashtree_insert(mem, &context->external_object_declarations,
                (kefir_hashtree_key_t) id, (kefir_hashtree_value_t) scoped_id));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_external(struct kefir_mem *mem,
                                                    struct kefir_ast_global_context *context,
                                                    const char *identifier,
                                                    const struct kefir_ast_type *type,
                                                    struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    bool external_declaration_exists = false;
    if (kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier)) {
        struct kefir_hashtree_node *external_node;
        REQUIRE_OK(kefir_hashtree_at(&context->external_object_declarations, (kefir_hashtree_key_t) identifier, &external_node));
        scoped_id = (struct kefir_ast_scoped_identifier *) external_node->value;
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        external_declaration_exists = true;
    }

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
            scoped_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    if (external_declaration_exists) {
        REQUIRE_OK(kefir_hashtree_delete(mem, &context->external_object_declarations, (kefir_hashtree_key_t) identifier));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_external_thread_local(struct kefir_mem *mem,
                                                                 struct kefir_ast_global_context *context,
                                                                 const char *identifier,
                                                                 const struct kefir_ast_type *type,
                                                                 struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    bool external_declaration_exists = false;
    if (kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier)) {
        struct kefir_hashtree_node *external_node;
        REQUIRE_OK(kefir_hashtree_at(&context->external_object_declarations, (kefir_hashtree_key_t) identifier, &external_node));
        scoped_id = (struct kefir_ast_scoped_identifier *) external_node->value;
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        external_declaration_exists = true;
    }

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
            scoped_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    if (external_declaration_exists) {
        REQUIRE_OK(kefir_hashtree_delete(mem, &context->external_object_declarations, (kefir_hashtree_key_t) identifier));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_static(struct kefir_mem *mem,
                                                  struct kefir_ast_global_context *context,
                                                  const char *identifier,
                                                  const struct kefir_ast_type *type,
                                                  struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE(!kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier),
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Identifier with static storage duration cannot be external"));

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
            scoped_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;   
}

kefir_result_t kefir_ast_global_context_define_static_thread_local(struct kefir_mem *mem,
                                                               struct kefir_ast_global_context *context,
                                                               const char *identifier,
                                                               const struct kefir_ast_type *type,
                                                               struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE(!kefir_hashtree_has(&context->external_object_declarations, (kefir_hashtree_key_t) identifier),
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Identifier with static storage duration cannot be external"));

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->object.type, type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->object.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
            scoped_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_object_identifier(mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_constant(struct kefir_mem *mem,
                                                    struct kefir_ast_global_context *context,
                                                    const char *identifier,
                                                    struct kefir_ast_constant_expression *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine constant");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_constant(mem, value);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_tag(struct kefir_mem *mem,
                                               struct kefir_ast_global_context *context,
                                               const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const char *identifier = NULL;
    REQUIRE_OK(kefir_ast_context_type_retrieve_tag(type, &identifier));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->tag_scope, identifier,
        (struct kefir_ast_scoped_identifier **) &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE_OK(kefir_ast_context_update_existing_scoped_type_tag(scoped_id, type));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_type_tag(mem, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->tag_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_type(struct kefir_mem *mem,
                                                struct kefir_ast_global_context *context,
                                                const char *identifier,
                                                const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier,
        (struct kefir_ast_scoped_identifier **) &scoped_id);
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
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_declare_function(struct kefir_mem *mem,
                                                     struct kefir_ast_global_context *context,
                                                     kefir_ast_function_specifier_t specifier,
                                                     const struct kefir_ast_type *function) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(function != NULL && function->tag == KEFIR_AST_TYPE_FUNCTION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(function->function_type.identifier != NULL && strlen(function->function_type.identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected function with non-empty identifier"));
    
    const char *identifier = function->function_type.identifier;
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    bool external_declaration_exists = false;
    if (kefir_hashtree_has(&context->external_function_declarations, (kefir_hashtree_key_t) identifier)) {
        struct kefir_hashtree_node *external_node;
        REQUIRE_OK(kefir_hashtree_at(&context->external_function_declarations, (kefir_hashtree_key_t) identifier, &external_node));
        scoped_id = (struct kefir_ast_scoped_identifier *) external_node->value;
        REQUIRE(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->function.type, function),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        external_declaration_exists = true;
    }

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
            scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->function.type, function),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->function.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
            scoped_id->function.type, function);
        scoped_id->function.specifier = kefir_ast_context_merge_function_specifiers(scoped_id->function.specifier, specifier);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_function_identifier(mem,
                function, specifier, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
        if (!external_declaration_exists) {
            REQUIRE_OK(kefir_hashtree_insert(mem, &context->external_function_declarations,
                (kefir_hashtree_key_t) id, (kefir_hashtree_value_t) scoped_id));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_function(struct kefir_mem *mem,
                                                    struct kefir_ast_global_context *context,
                                                    kefir_ast_function_specifier_t specifier,
                                                    const struct kefir_ast_type *function) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(function != NULL && function->tag == KEFIR_AST_TYPE_FUNCTION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(function->function_type.identifier != NULL && strlen(function->function_type.identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected function with non-empty identifier"));
    
    const char *identifier = function->function_type.identifier;
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    bool external_declaration_exists = false;
    if (kefir_hashtree_has(&context->external_function_declarations, (kefir_hashtree_key_t) identifier)) {
        struct kefir_hashtree_node *external_node;
        REQUIRE_OK(kefir_hashtree_at(&context->external_function_declarations, (kefir_hashtree_key_t) identifier, &external_node));
        scoped_id = (struct kefir_ast_scoped_identifier *) external_node->value;
        REQUIRE(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->function.type, function),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        external_declaration_exists = true;
    }

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
            scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->function.type, function),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->function.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
            scoped_id->function.type, function);
        scoped_id->function.specifier = kefir_ast_context_merge_function_specifiers(scoped_id->function.specifier, specifier);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_function_identifier(mem,
                function, specifier, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    if (external_declaration_exists) {
        REQUIRE_OK(kefir_hashtree_delete(mem, &context->external_function_declarations, (kefir_hashtree_key_t) identifier));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_static_function(struct kefir_mem *mem,
                                                           struct kefir_ast_global_context *context,
                                                           kefir_ast_function_specifier_t specifier,
                                                           const struct kefir_ast_type *function) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(function != NULL && function->tag == KEFIR_AST_TYPE_FUNCTION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(function->function_type.identifier != NULL && strlen(function->function_type.identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected function with non-empty identifier"));
    
    const char *identifier = function->function_type.identifier;
    REQUIRE(!kefir_hashtree_has(&context->external_function_declarations, (kefir_hashtree_key_t) identifier),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function identifier with static storage cannot be external"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
        REQUIRE(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, scoped_id->function.type, function),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "All declarations of the same identifier shall have compatible types"));
        scoped_id->function.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
            scoped_id->function.type, function);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *scoped_id =
            kefir_ast_context_allocate_scoped_function_identifier(mem,
                function, specifier, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    return KEFIR_OK;
}