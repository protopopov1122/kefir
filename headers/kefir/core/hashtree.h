#ifndef KEFIR_CORE_HASHTREE_H_
#define KEFIR_CORE_HASHTREE_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef kefir_uint64_t kefir_hashtree_hash_t;

typedef struct kefir_hashtree_node {
    const char *key;
    kefir_hashtree_hash_t hash;
    void *value;

    kefir_size_t height;
    struct kefir_hashtree_node *left_child;
    struct kefir_hashtree_node *right_child;
} kefir_hashtree_node_t;

typedef struct kefir_hashtree {
    struct kefir_hashtree_node *root;
    struct {
        kefir_result_t (*callback)(struct kefir_mem *, struct kefir_hashtree *, const char *, void *, void *);
        void *data;
    } node_remove;
} kefir_hashtree_t;

typedef kefir_result_t (*kefir_hashtree_traversal_t)(const struct kefir_hashtree *, const char *, void *, void *);

kefir_result_t kefir_hashtree_init(struct kefir_hashtree *);
kefir_result_t kefir_hashtree_on_removal(struct kefir_hashtree *,
                                     kefir_result_t (*)(struct kefir_mem *, struct kefir_hashtree *, const char *, void *, void *),
                                     void *);
kefir_result_t kefir_hashtree_free(struct kefir_mem *, struct kefir_hashtree *);
kefir_result_t kefir_hashtree_insert(struct kefir_mem *, struct kefir_hashtree *, const char *, void *);
kefir_result_t kefir_hashtree_emplace(struct kefir_mem *, struct kefir_hashtree *, const char *, void *, void **);
kefir_result_t kefir_hashtree_at(const struct kefir_hashtree *, const char *, struct kefir_hashtree_node **);
kefir_result_t kefir_hashtree_traverse(const struct kefir_hashtree *, kefir_hashtree_traversal_t, void *);

#endif