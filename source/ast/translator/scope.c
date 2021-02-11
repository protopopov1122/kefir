#include "kefir/ast/translator/scope.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_identifier_scope_init(struct kefir_ast_identifier_scope *scope) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE_OK(kefir_hashtree_init(&scope->content, &kefir_hashtree_str_ops));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_scope_free(struct kefir_mem *mem,
                                           struct kefir_ast_identifier_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE_OK(kefir_hashtree_free(mem, &scope->content));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_scope_insert(struct kefir_mem *mem,
                                             struct kefir_ast_identifier_scope *scope,
                                             const char *identifier,
                                             const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE_OK(kefir_hashtree_insert(mem, &scope->content, (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) type));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_scope_at(const struct kefir_ast_identifier_scope *scope,
                                         const char *identifier,
                                         const struct kefir_ast_type **type) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type pointer"));
    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&scope->content, (kefir_hashtree_key_t) identifier, &node));
    *type = (struct kefir_ast_type *) node->value;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_scope_iter(const struct kefir_ast_identifier_scope *scope,
                                           struct kefir_ast_identifier_scope_iterator *iter,
                                           const char **identifier,
                                           const struct kefir_ast_type **type) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope iterator"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type pointer"));
    const struct kefir_hashtree_node *node = kefir_hashtree_iter(&scope->content, &iter->iter);
    if (node != NULL) {
        *identifier = (const char *) node->key;
        *type = (const struct kefir_ast_type *) node->value;
        return KEFIR_OK;
    } else {
        *identifier = NULL;
        *type = NULL;
        return KEFIR_ITERATOR_END;
    }
}

kefir_result_t kefir_ast_identifier_scope_next(const struct kefir_ast_identifier_scope *scope,
                                           struct kefir_ast_identifier_scope_iterator *iter,
                                           const char **identifier,
                                           const struct kefir_ast_type **type) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope iterator"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type pointer"));
    const struct kefir_hashtree_node *node = kefir_hashtree_next(&iter->iter);
    if (node != NULL) {
        *identifier = (const char *) node->key;
        *type = (const struct kefir_ast_type *) node->value;
        return KEFIR_OK;
    } else {
        *identifier = NULL;
        *type = NULL;
        return KEFIR_ITERATOR_END;
    }
}

static kefir_result_t multi_scope_remove(struct kefir_mem *mem, void *raw_scope, void *payload) {
    UNUSED(payload);
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_scope *, scope,
        raw_scope);
    REQUIRE_OK(kefir_ast_identifier_scope_free(mem, scope));
    KEFIR_FREE(mem, scope);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_multi_scope_init(struct kefir_mem *mem,
                                      struct kefir_ast_multi_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    struct kefir_ast_identifier_scope *root_scope = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_identifier_scope));
    REQUIRE(root_scope != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate identifier scope"));
    kefir_result_t res = kefir_ast_identifier_scope_init(root_scope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, root_scope);
        return res;
    });
    res = kefir_tree_init(&scope->root, root_scope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_identifier_scope_free(mem, root_scope);
        KEFIR_FREE(mem, root_scope);
        return res;
    });
    res = kefir_tree_on_removal(&scope->root, multi_scope_remove, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_identifier_scope_free(mem, root_scope);
        KEFIR_FREE(mem, root_scope);
        return res;
    });
    scope->top_scope = &scope->root;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_multi_scope_free(struct kefir_mem *mem,
                                      struct kefir_ast_multi_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    REQUIRE_OK(kefir_tree_free(mem, &scope->root));
    scope->top_scope = NULL;
    return KEFIR_OK;
}

struct kefir_ast_identifier_scope *kefir_ast_multi_scope_top(const struct kefir_ast_multi_scope *scope) {
    REQUIRE(scope != NULL, NULL);
    return (struct kefir_ast_identifier_scope *) scope->top_scope;
}

kefir_result_t kefir_ast_multi_scope_open(struct kefir_mem *mem,
                                      struct kefir_ast_multi_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    struct kefir_ast_identifier_scope *subscope = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_identifier_scope));
    REQUIRE(subscope != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate identifier scope"));
    kefir_result_t res = kefir_ast_identifier_scope_init(subscope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, subscope);
        return res;
    });
    struct kefir_tree_node *node = NULL;
    res = kefir_tree_insert_child(mem, scope->top_scope, subscope, &node);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_identifier_scope_free(mem, subscope);
        KEFIR_FREE(mem, subscope);
        return res;
    });
    scope->top_scope = node;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_multi_scope_close(struct kefir_ast_multi_scope *scope) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    REQUIRE(scope->top_scope->parent != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot close root scope"));
    scope->top_scope = scope->top_scope->parent;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_multi_scope_insert(struct kefir_mem *mem,
                                        const struct kefir_ast_multi_scope *scope,
                                        const char *identifier,
                                        const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    struct kefir_ast_identifier_scope *top_scope = kefir_ast_multi_scope_top(scope);
    REQUIRE(top_scope, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Failed to retieve current identifier scope"));
    REQUIRE_OK(kefir_ast_identifier_scope_insert(mem, top_scope, identifier, type));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_multi_scope_at(const struct kefir_ast_multi_scope *scope,
                                    const char *identifier,
                                    const struct kefir_ast_type **type) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    struct kefir_tree_node *current_node = scope->top_scope;
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_scope *, current_scope,
        current_node->value);
    REQUIRE(current_scope, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Failed to retieve current identifier scope"));
    while (current_scope != NULL) {
        kefir_result_t res = kefir_ast_identifier_scope_at(current_scope, identifier, type);
        if (res == KEFIR_NOT_FOUND) {
            current_node = current_node->parent;
            current_scope = (struct kefir_ast_identifier_scope *) current_node->value;
        } else {
            return res;
        }
    }
    return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Unable to find specified type");
}
