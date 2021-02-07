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