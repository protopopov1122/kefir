#include <string.h>
#include "kefir/core/hashtree.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t node_free(struct kefir_mem *mem,
                              struct kefir_hashtree *tree,
                              struct kefir_hashtree_node *node) {
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

static struct kefir_hashtree_node *node_alloc(struct kefir_mem *mem,
                                            kefir_hashtree_hash_t hash,
                                            const char *key,
                                            void *value) {
    struct kefir_hashtree_node *node = KEFIR_MALLOC(mem, sizeof(struct kefir_hashtree_node));
    REQUIRE_ELSE(node != NULL, {
        return NULL;
    });
    node->hash = hash;
    node->key = key;
    node->value = value;
    node->height = 0;
    node->left_child = NULL;
    node->right_child = NULL;
    return node;
}

static kefir_result_t node_insert(struct kefir_mem *mem,
                        struct kefir_hashtree *tree,
                        struct kefir_hashtree_node *root,
                        kefir_hashtree_hash_t hash,
                        const char *key,
                        void *value,
                        void **oldvalue,
                        bool replace) {
    if (root->hash < hash) {
        if (root->left_child == NULL) {
            struct kefir_hashtree_node *node = node_alloc(mem, hash, key, value);
            REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate hash tree node"));
            root->left_child = node;
            return KEFIR_OK;
        } else {
            return node_insert(mem, tree, root->left_child, hash, key, value, oldvalue, replace);
        }
    } else if (root->hash > hash ||
        strcmp(root->key, key) != 0) {
        if (root->right_child == NULL) {
            struct kefir_hashtree_node *node = node_alloc(mem, hash, key, value);
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

static kefir_result_t node_find(struct kefir_hashtree_node *root,
                              const struct kefir_hashtree *tree,
                              kefir_hashtree_hash_t hash,
                              const char *key,
                              void **result) {
    if (root == NULL) {
        return KEFIR_NOT_FOUND;
    }
    if (root->hash < hash) {
        return node_find(root->left_child, tree, hash, key, result);
    } else if (root->hash > hash ||
        strcmp(root->key, key) != 0) {
        return node_find(root->right_child, tree, hash, key, result);
    } else {
        *result = root->value;
        return KEFIR_OK;
    }
}

static kefir_hashtree_hash_t str_hash(const char *str) {
    REQUIRE(str != NULL, 0);
    kefir_hashtree_hash_t hash = 7;
    const kefir_size_t length = strlen(str);
    for (kefir_size_t i = 0; i < length; i++) {
        hash = (hash * 31) + str[i];
    }
    return hash;
}

static kefir_result_t node_traverse(const struct kefir_hashtree *tree,
                                  struct kefir_hashtree_node *node,
                                  kefir_hashtree_traversal_t traverse,
                                  void *data) {
    if (node == NULL) {
        return KEFIR_OK;
    }
    REQUIRE_OK(node_traverse(tree, node->left_child, traverse, data));
    REQUIRE_OK(traverse(tree, node->key, node->value, data));
    REQUIRE_OK(node_traverse(tree, node->right_child, traverse, data));
    return KEFIR_OK;
}

kefir_result_t kefir_hashtree_init(struct kefir_hashtree *tree) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    tree->root = NULL;
    tree->node_remove.callback = NULL;
    tree->node_remove.data = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_hashtree_on_removal(struct kefir_hashtree *tree,
                                     kefir_result_t (*callback)(struct kefir_mem *, struct kefir_hashtree *, const char *, void *, void *),
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

kefir_result_t kefir_hashtree_insert(struct kefir_mem *mem, struct kefir_hashtree *tree, const char *key, void *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    kefir_hashtree_hash_t hash = str_hash(key);
    if (tree->root == NULL) {
        tree->root = node_alloc(mem, hash, key, value);
        return KEFIR_OK;
    } else {
        return node_insert(mem, tree, tree->root, hash, key, value, NULL, false);
    }
}

kefir_result_t kefir_hashtree_emplace(struct kefir_mem *mem, struct kefir_hashtree *tree, const char *key, void *value, void **oldvalue) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    kefir_hashtree_hash_t hash = str_hash(key);
    if (tree->root == NULL) {
        tree->root = node_alloc(mem, hash, key, value);
        return KEFIR_OK;
    } else {
        if (oldvalue != NULL) {
            *oldvalue = NULL;
        }
        return node_insert(mem, tree, tree->root, hash, key, value, oldvalue, true);
    }
}

kefir_result_t kefir_hashtree_at(const struct kefir_hashtree *tree, const char *key, void **result) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid result pointer"));
    *result = NULL;
    kefir_hashtree_hash_t hash = str_hash(key);
    if (node_find(tree->root, tree, hash, key, result) != KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Could not find tree node with specified key");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_hashtree_traverse(const struct kefir_hashtree *tree, kefir_hashtree_traversal_t traverse, void *data) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    return node_traverse(tree, tree->root, traverse, data);
}