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
                                            kefir_hashtree_key_t key,
                                            kefir_hashtree_value_t value) {
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
                        kefir_hashtree_key_t key,
                        kefir_hashtree_value_t value,
                        kefir_hashtree_value_t *oldvalue,
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
        !tree->ops->compare_keys(root->key, key, tree->ops->data)) {
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
                              kefir_hashtree_key_t key,
                              struct kefir_hashtree_node **result) {
    if (root == NULL) {
        return KEFIR_NOT_FOUND;
    }
    if (root->hash < hash) {
        return node_find(root->left_child, tree, hash, key, result);
    } else if (root->hash > hash ||
        !tree->ops->compare_keys(root->key, key, tree->ops->data)) {
        return node_find(root->right_child, tree, hash, key, result);
    } else {
        *result = root;
        return KEFIR_OK;
    }
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
                                     kefir_result_t (*callback)(struct kefir_mem *, struct kefir_hashtree *, kefir_hashtree_key_t, kefir_hashtree_value_t, void *),
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

kefir_result_t kefir_hashtree_insert(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key, kefir_hashtree_value_t value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    kefir_hashtree_hash_t hash = tree->ops->hash(key, tree->ops->data);
    if (tree->root == NULL) {
        tree->root = node_alloc(mem, hash, key, value);
        return KEFIR_OK;
    } else {
        return node_insert(mem, tree, tree->root, hash, key, value, NULL, false);
    }
}

kefir_result_t kefir_hashtree_at(const struct kefir_hashtree *tree, kefir_hashtree_key_t key, struct kefir_hashtree_node **result) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid result pointer"));
    *result = NULL;
    kefir_hashtree_hash_t hash = tree->ops->hash(key, tree->ops->data);
    if (node_find(tree->root, tree, hash, key, result) != KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Could not find tree node with specified key");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_hashtree_traverse(const struct kefir_hashtree *tree, kefir_hashtree_traversal_t traverse, void *data) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash tree pointer"));
    return node_traverse(tree, tree->root, traverse, data);
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
    return (str1 == NULL && str2 == NULL) ||
        strcmp(str1, str2) == 0;
}

const struct kefir_hashtree_ops kefir_hashtree_str_ops = {
    .hash = str_hash,
    .compare_keys = str_compare,
    .data = NULL
};

static kefir_hashtree_hash_t uint_hash(kefir_hashtree_key_t key, void *data) {
    UNUSED(data);
    return (kefir_hashtree_hash_t) key;
}

static bool uint_compare(kefir_hashtree_key_t key1, kefir_hashtree_key_t key2, void *data) {
    UNUSED(data);
    return key1 == key2;
}

const struct kefir_hashtree_ops kefir_hashtree_uint_ops = {
    .hash = uint_hash,
    .compare_keys = uint_compare,
    .data = NULL
};