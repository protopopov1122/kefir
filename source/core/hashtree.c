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
#include "kefir/core/hashtree.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t node_free(struct kefir_mem *mem, struct kefir_hashtree *tree, struct kefir_hashtree_node *node) {
    if (node == NULL) {
        return KEFIR_OK;
    }
    REQUIRE_OK(node_free(mem, tree, node->left_child));
    REQUIRE_OK(node_free(mem, tree, node->right_child));
    if (tree->node_remove.callback != NULL) {
        REQUIRE_OK(tree->node_remove.callback(mem, tree, node->key, node->value, tree->node_remove.data));
    }
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

static struct kefir_hashtree_node *node_alloc(struct kefir_mem *mem, kefir_hashtree_hash_t hash,
                                              struct kefir_hashtree_node *parent, kefir_hashtree_key_t key,
                                              kefir_hashtree_value_t value) {
    struct kefir_hashtree_node *node = KEFIR_MALLOC(mem, sizeof(struct kefir_hashtree_node));
    REQUIRE_ELSE(node != NULL, { return NULL; });
    node->hash = hash;
    node->key = key;
    node->value = value;
    node->height = 0;
    node->parent = parent;
    node->left_child = NULL;
    node->right_child = NULL;
    return node;
}

static kefir_result_t node_insert(struct kefir_mem *mem, struct kefir_hashtree *tree, struct kefir_hashtree_node *root,
                                  kefir_hashtree_hash_t hash, kefir_hashtree_key_t key, kefir_hashtree_value_t value,
                                  kefir_hashtree_value_t *oldvalue, bool replace) {
    if (hash < root->hash) {
        if (root->left_child == NULL) {
            struct kefir_hashtree_node *node = node_alloc(mem, hash, root, key, value);
            REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate hash tree node"));
            root->left_child = node;
            return KEFIR_OK;
        } else {
            return node_insert(mem, tree, root->left_child, hash, key, value, oldvalue, replace);
        }
    } else if (hash > root->hash || !tree->ops->compare_keys(root->key, key, tree->ops->data)) {
        if (root->right_child == NULL) {
            struct kefir_hashtree_node *node = node_alloc(mem, hash, root, key, value);
            REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate hash tree node"));
            root->right_child = node;
            return KEFIR_OK;
        } else {
            return node_insert(mem, tree, root->right_child, hash, key, value, oldvalue, replace);
        }
    } else if (replace) {
        if (oldvalue != NULL) {
            *oldvalue = root->value;
        }
        root->value = value;
        return KEFIR_OK;
    } else {
        return KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Hash tree node with specified key already exists in the tree");
    }
}

static kefir_result_t node_find(struct kefir_hashtree_node *root, const struct kefir_hashtree *tree,
                                kefir_hashtree_hash_t hash, kefir_hashtree_key_t key,
                                struct kefir_hashtree_node **result) {
    if (root == NULL) {
        return KEFIR_NOT_FOUND;
    }
    if (hash < root->hash) {
        return node_find(root->left_child, tree, hash, key, result);
    } else if (hash < root->hash || !tree->ops->compare_keys(root->key, key, tree->ops->data)) {
        return node_find(root->right_child, tree, hash, key, result);
    } else {
        *result = root;
        return KEFIR_OK;
    }
}

static struct kefir_hashtree_node *node_minimum(struct kefir_hashtree_node *root) {
    if (root == NULL || root->left_child == NULL) {
        return root;
    } else {
        return node_minimum(root->left_child);
    }
}

kefir_result_t kefir_hashtree_init(struct kefir_hashtree *tree, const struct kefir_hashtree_ops *ops) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    REQUIRE(ops != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree operation pointer"));
    tree->ops = ops;
    tree->root = NULL;
    tree->node_remove.callback = NULL;
    tree->node_remove.data = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_hashtree_on_removal(struct kefir_hashtree *tree,
                                         kefir_result_t (*callback)(struct kefir_mem *, struct kefir_hashtree *,
                                                                    kefir_hashtree_key_t, kefir_hashtree_value_t,
                                                                    void *),
                                         void *data) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    tree->node_remove.callback = callback;
    tree->node_remove.data = data;
    return KEFIR_OK;
}

kefir_result_t kefir_hashtree_free(struct kefir_mem *mem, struct kefir_hashtree *tree) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    REQUIRE_OK(node_free(mem, tree, tree->root));
    tree->root = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_hashtree_insert(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key,
                                     kefir_hashtree_value_t value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    kefir_hashtree_hash_t hash = tree->ops->hash(key, tree->ops->data);
    if (tree->root == NULL) {
        tree->root = node_alloc(mem, hash, NULL, key, value);
        return KEFIR_OK;
    } else {
        return node_insert(mem, tree, tree->root, hash, key, value, NULL, false);
    }
}

kefir_result_t kefir_hashtree_at(const struct kefir_hashtree *tree, kefir_hashtree_key_t key,
                                 struct kefir_hashtree_node **result) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid result pointer"));
    *result = NULL;
    kefir_hashtree_hash_t hash = tree->ops->hash(key, tree->ops->data);
    if (node_find(tree->root, tree, hash, key, result) != KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Could not find tree node with specified key");
    }
    return KEFIR_OK;
}

kefir_bool_t kefir_hashtree_has(const struct kefir_hashtree *tree, kefir_hashtree_key_t key) {
    REQUIRE(tree != NULL, false);
    kefir_hashtree_hash_t hash = tree->ops->hash(key, tree->ops->data);
    struct kefir_hashtree_node *result = NULL;
    return node_find(tree->root, tree, hash, key, &result) == KEFIR_OK;
}

kefir_bool_t kefir_hashtree_empty(const struct kefir_hashtree *tree) {
    REQUIRE(tree != NULL, true);
    return tree->root == NULL;
}

kefir_result_t kefir_hashtree_delete(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    kefir_hashtree_hash_t hash = tree->ops->hash(key, tree->ops->data);
    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(node_find(tree->root, tree, hash, key, &node));
    struct kefir_hashtree_node *replacement = NULL;
    if (node->left_child == NULL && node->right_child == NULL) {
        // Do nothing
    } else if (node->left_child != NULL && node->right_child == NULL) {
        replacement = node->left_child;
    } else if (node->left_child == NULL && node->right_child != NULL) {
        replacement = node->right_child;
    } else {
        replacement = node_minimum(node->right_child);
        REQUIRE(replacement != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Internal hash tree removal error"));
        replacement->left_child = node->left_child;
        replacement->left_child->parent = replacement;
        if (replacement->parent != node) {
            if (replacement->right_child != NULL) {
                replacement->parent->left_child = replacement->right_child;
                replacement->right_child = NULL;
            }
            replacement->right_child = node->right_child;
            replacement->right_child->parent = replacement;
        }
    }
    if (node->parent != NULL) {
        if (node->parent->left_child == node) {
            node->parent->left_child = replacement;
        } else if (node->parent->right_child == node) {
            node->parent->right_child = replacement;
        } else {
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Malformed binary tree");
        }
    } else {
        tree->root = replacement;
    }
    if (replacement != NULL) {
        replacement->parent = node->parent;
    }
    if (tree->node_remove.callback != NULL) {
        REQUIRE_OK(tree->node_remove.callback(mem, tree, node->key, node->value, tree->node_remove.data));
    }
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

kefir_result_t kefir_hashtree_clean(struct kefir_mem *mem, struct kefir_hashtree *tree) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    REQUIRE_OK(node_free(mem, tree, tree->root));
    tree->root = NULL;
    return KEFIR_OK;
}

static const struct kefir_hashtree_node *min_node(const struct kefir_hashtree_node *node) {
    if (node == NULL) {
        return NULL;
    }
    const struct kefir_hashtree_node *child = min_node(node->left_child);
    if (child != NULL) {
        return child;
    } else {
        return node;
    }
}

const struct kefir_hashtree_node *kefir_hashtree_iter(const struct kefir_hashtree *tree,
                                                      struct kefir_hashtree_node_iterator *iter) {
    REQUIRE(tree != NULL, NULL);
    REQUIRE(iter != NULL, NULL);
    iter->node = min_node(tree->root);
    iter->last_hash = 0;
    if (iter->node != NULL) {
        iter->last_hash = iter->node->hash;
    }
    return iter->node;
}

const struct kefir_hashtree_node *kefir_hashtree_next(struct kefir_hashtree_node_iterator *iter) {
    REQUIRE(iter != NULL, NULL);
    REQUIRE(iter->node != NULL, NULL);
    if (iter->node->right_child != NULL) {
        iter->node = min_node(iter->node->right_child);
        iter->last_hash = iter->node->hash;
        return iter->node;
    }
    while (iter->node->parent != NULL) {
        iter->node = iter->node->parent;
        if (iter->node->hash > iter->last_hash) {
            iter->last_hash = iter->node->hash;
            return iter->node;
        }
    }
    iter->node = NULL;
    iter->last_hash = 0;
    return iter->node;
}

static kefir_hashtree_hash_t str_hash(kefir_hashtree_key_t key, void *data) {
    UNUSED(data);
    const char *str = (const char *) key;
    REQUIRE(str != NULL, 0);
    kefir_hashtree_hash_t hash = 7;
    const kefir_size_t length = strlen(str);
    for (kefir_size_t i = 0; i < length; i++) {
        hash = (hash * 31) + str[i];
    }
    return hash;
}

static bool str_compare(kefir_hashtree_key_t key1, kefir_hashtree_key_t key2, void *data) {
    UNUSED(data);
    const char *str1 = (const char *) key1;
    const char *str2 = (const char *) key2;
    return (str1 == NULL && str2 == NULL) || strcmp(str1, str2) == 0;
}

const struct kefir_hashtree_ops kefir_hashtree_str_ops = {.hash = str_hash, .compare_keys = str_compare, .data = NULL};

static kefir_hashtree_hash_t uint_hash(kefir_hashtree_key_t key, void *data) {
    UNUSED(data);
    return (kefir_hashtree_hash_t) key;
}

static bool uint_compare(kefir_hashtree_key_t key1, kefir_hashtree_key_t key2, void *data) {
    UNUSED(data);
    return key1 == key2;
}

const struct kefir_hashtree_ops kefir_hashtree_uint_ops = {
    .hash = uint_hash, .compare_keys = uint_compare, .data = NULL};
