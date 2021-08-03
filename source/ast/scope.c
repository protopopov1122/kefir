/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "kefir/ast/scope.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t flat_scope_removal(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key,
                                         kefir_hashtree_value_t value, void *payload) {
    UNUSED(tree);
    UNUSED(key);
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, scope, payload);
    if (scope->remove_callback != NULL) {
        REQUIRE_OK(scope->remove_callback(mem, (struct kefir_ast_scoped_identifier *) value, scope->remove_payload));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_flat_scope_init(struct kefir_ast_identifier_flat_scope *scope) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    scope->remove_callback = NULL;
    scope->remove_payload = NULL;
    REQUIRE_OK(kefir_hashtree_init(&scope->content, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&scope->content, flat_scope_removal, scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_flat_scope_free(struct kefir_mem *mem,
                                                    struct kefir_ast_identifier_flat_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE_OK(kefir_hashtree_free(mem, &scope->content));
    scope->remove_callback = NULL;
    scope->remove_payload = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_flat_scope_cleanup_payload(struct kefir_mem *mem,
                                                               const struct kefir_ast_identifier_flat_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));

    struct kefir_ast_identifier_flat_scope_iterator iter;
    kefir_result_t res = KEFIR_OK;
    for (res = kefir_ast_identifier_flat_scope_iter(scope, &iter); res == KEFIR_OK;
         res = kefir_ast_identifier_flat_scope_next(scope, &iter)) {

        if (iter.value->cleanup.callback != NULL) {
            REQUIRE_OK(iter.value->cleanup.callback(mem, iter.value, iter.value->cleanup.payload));
            iter.value->cleanup.callback = NULL;
            iter.value->cleanup.payload = NULL;
        }
    }
    REQUIRE(res == KEFIR_ITERATOR_END, res);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_flat_scope_on_removal(
    struct kefir_ast_identifier_flat_scope *scope,
    kefir_result_t (*callback)(struct kefir_mem *, struct kefir_ast_scoped_identifier *, void *), void *payload) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    scope->remove_callback = callback;
    scope->remove_payload = payload;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_flat_scope_insert(struct kefir_mem *mem,
                                                      struct kefir_ast_identifier_flat_scope *scope,
                                                      const char *identifier,
                                                      struct kefir_ast_scoped_identifier *scoped_identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier"));
    REQUIRE_OK(kefir_hashtree_insert(mem, &scope->content, (kefir_hashtree_key_t) identifier,
                                     (kefir_hashtree_value_t) scoped_identifier));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_flat_scope_at(const struct kefir_ast_identifier_flat_scope *scope,
                                                  const char *identifier,
                                                  struct kefir_ast_scoped_identifier **scope_identifier) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier"));
    REQUIRE(scope_identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&scope->content, (kefir_hashtree_key_t) identifier, &node));
    *scope_identifier = (struct kefir_ast_scoped_identifier *) node->value;
    return KEFIR_OK;
}

kefir_bool_t kefir_ast_identifier_flat_scope_has(const struct kefir_ast_identifier_flat_scope *scope,
                                                 const char *identifier) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier"));
    return kefir_hashtree_has(&scope->content, (kefir_hashtree_key_t) identifier);
}

kefir_bool_t kefir_ast_identifier_flat_scope_empty(const struct kefir_ast_identifier_flat_scope *scope) {
    REQUIRE(scope != NULL, true);
    return kefir_hashtree_empty(&scope->content);
}

kefir_result_t kefir_ast_identifier_flat_scope_iter(const struct kefir_ast_identifier_flat_scope *scope,
                                                    struct kefir_ast_identifier_flat_scope_iterator *iter) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope iterator"));
    const struct kefir_hashtree_node *node = kefir_hashtree_iter(&scope->content, &iter->iter);
    if (node != NULL) {
        iter->identifier = (const char *) node->key;
        iter->value = (struct kefir_ast_scoped_identifier *) node->value;
        return KEFIR_OK;
    } else {
        iter->identifier = NULL;
        iter->value = NULL;
        return KEFIR_ITERATOR_END;
    }
}

kefir_result_t kefir_ast_identifier_flat_scope_next(const struct kefir_ast_identifier_flat_scope *scope,
                                                    struct kefir_ast_identifier_flat_scope_iterator *iter) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier scope iterator"));
    const struct kefir_hashtree_node *node = kefir_hashtree_next(&iter->iter);
    if (node != NULL) {
        iter->identifier = (const char *) node->key;
        iter->value = (struct kefir_ast_scoped_identifier *) node->value;
        return KEFIR_OK;
    } else {
        iter->identifier = NULL;
        iter->value = NULL;
        return KEFIR_ITERATOR_END;
    }
}

static kefir_result_t multi_scope_remove(struct kefir_mem *mem, void *raw_scope, void *payload) {
    UNUSED(payload);
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, scope, raw_scope);
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, scope));
    KEFIR_FREE(mem, scope);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_block_scope_init(struct kefir_mem *mem,
                                                     struct kefir_ast_identifier_block_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    struct kefir_ast_identifier_flat_scope *root_scope =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_identifier_flat_scope));
    REQUIRE(root_scope != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate identifier scope"));
    kefir_result_t res = kefir_ast_identifier_flat_scope_init(root_scope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, root_scope);
        return res;
    });
    res = kefir_tree_init(&scope->root, root_scope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_identifier_flat_scope_free(mem, root_scope);
        KEFIR_FREE(mem, root_scope);
        return res;
    });
    res = kefir_tree_on_removal(&scope->root, multi_scope_remove, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_identifier_flat_scope_free(mem, root_scope);
        KEFIR_FREE(mem, root_scope);
        return res;
    });
    scope->top_scope = &scope->root;
    scope->remove_callback = NULL;
    scope->remove_payload = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_block_scope_free(struct kefir_mem *mem,
                                                     struct kefir_ast_identifier_block_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    REQUIRE_OK(kefir_tree_free(mem, &scope->root));
    scope->top_scope = NULL;
    return KEFIR_OK;
}

static kefir_result_t invoke_cleanup_callbacks(struct kefir_mem *mem, const struct kefir_tree_node *root) {
    REQUIRE(root != NULL, KEFIR_OK);
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, flat_scope, root->value);
    REQUIRE_OK(kefir_ast_identifier_flat_scope_cleanup_payload(mem, flat_scope));
    for (struct kefir_tree_node *child = kefir_tree_first_child(root); child != NULL;
         child = kefir_tree_next_sibling(child)) {
        REQUIRE_OK(invoke_cleanup_callbacks(mem, child));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_block_scope_cleanup_payload(struct kefir_mem *mem,
                                                                const struct kefir_ast_identifier_block_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    REQUIRE_OK(invoke_cleanup_callbacks(mem, &scope->root));
    return KEFIR_OK;
}

static kefir_result_t update_removal_callback(struct kefir_ast_identifier_block_scope *scope,
                                              struct kefir_tree_node *root) {
    REQUIRE(root != NULL, KEFIR_OK);
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, flat_scope, root->value);
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(flat_scope, scope->remove_callback, scope->remove_payload));
    for (struct kefir_tree_node *child = kefir_tree_first_child(root); child != NULL;
         child = kefir_tree_next_sibling(child)) {
        REQUIRE_OK(update_removal_callback(scope, child));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_block_scope_on_removal(
    struct kefir_ast_identifier_block_scope *scope,
    kefir_result_t (*callback)(struct kefir_mem *, struct kefir_ast_scoped_identifier *, void *), void *payload) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    scope->remove_callback = callback;
    scope->remove_payload = payload;
    REQUIRE_OK(update_removal_callback(scope, &scope->root));
    return KEFIR_OK;
}

struct kefir_ast_identifier_flat_scope *kefir_ast_identifier_block_scope_top(
    const struct kefir_ast_identifier_block_scope *scope) {
    REQUIRE(scope != NULL, NULL);
    return (struct kefir_ast_identifier_flat_scope *) scope->top_scope->value;
}

kefir_result_t kefir_ast_identifier_block_scope_push(struct kefir_mem *mem,
                                                     struct kefir_ast_identifier_block_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    struct kefir_ast_identifier_flat_scope *subscope =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_identifier_flat_scope));
    REQUIRE(subscope != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate identifier scope"));
    kefir_result_t res = kefir_ast_identifier_flat_scope_init(subscope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, subscope);
        return res;
    });
    if (scope->remove_callback != NULL) {
        res = kefir_ast_identifier_flat_scope_on_removal(subscope, scope->remove_callback, scope->remove_payload);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_identifier_flat_scope_free(mem, subscope);
            KEFIR_FREE(mem, subscope);
            return res;
        });
    }
    struct kefir_tree_node *node = NULL;
    res = kefir_tree_insert_child(mem, scope->top_scope, subscope, &node);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_identifier_flat_scope_free(mem, subscope);
        KEFIR_FREE(mem, subscope);
        return res;
    });
    scope->top_scope = node;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_block_scope_pop(struct kefir_ast_identifier_block_scope *scope) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    REQUIRE(scope->top_scope->parent != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot close root scope"));
    scope->top_scope = scope->top_scope->parent;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_block_scope_insert(struct kefir_mem *mem,
                                                       const struct kefir_ast_identifier_block_scope *scope,
                                                       const char *identifier,
                                                       struct kefir_ast_scoped_identifier *scoped_identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    struct kefir_ast_identifier_flat_scope *top_scope = kefir_ast_identifier_block_scope_top(scope);
    REQUIRE(top_scope, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Failed to retieve current identifier scope"));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, top_scope, identifier, scoped_identifier));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_identifier_block_scope_at(const struct kefir_ast_identifier_block_scope *scope,
                                                   const char *identifier,
                                                   struct kefir_ast_scoped_identifier **scoped_identifier) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid multi scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    struct kefir_tree_node *current_node = scope->top_scope;
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, current_scope, current_node->value);
    REQUIRE(current_scope, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Failed to retieve current identifier scope"));
    while (current_scope != NULL) {
        kefir_result_t res = kefir_ast_identifier_flat_scope_at(current_scope, identifier, scoped_identifier);
        if (res == KEFIR_NOT_FOUND) {
            current_node = current_node->parent;
            if (current_node != NULL) {
                current_scope = (struct kefir_ast_identifier_flat_scope *) current_node->value;
            } else {
                current_scope = NULL;
            }
        } else {
            return res;
        }
    }
    return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Unable to find specified scoped identifier");
}

static kefir_bool_t block_tree_empty(const struct kefir_tree_node *node) {
    REQUIRE(node != NULL, true);
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, scope, node->value);
    if (!kefir_ast_identifier_flat_scope_empty(scope)) {
        return false;
    }
    for (struct kefir_tree_node *child = kefir_tree_first_child(node); child != NULL;
         child = kefir_tree_next_sibling(child)) {
        if (!block_tree_empty(child)) {
            return false;
        }
    }
    return true;
}

kefir_bool_t kefir_ast_identifier_block_scope_empty(const struct kefir_ast_identifier_block_scope *scope) {
    REQUIRE(scope != NULL, true);
    return block_tree_empty(&scope->root);
}
