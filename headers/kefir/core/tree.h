#ifndef KEFIR_CORE_TREE_H_
#define KEFIR_CORE_TREE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/list.h"
#include "kefir/core/mem.h"
typedef struct kefir_tree_node {
    void *value;
    struct kefir_tree_node *parent;
    struct kefir_tree_node *prev_sibling;
    struct kefir_tree_node *next_sibling;
    struct kefir_list children;

    kefir_result_t (*removal_callback)(struct kefir_mem *, void *, void *);
    void *removal_payload;
} kefir_tree_node_t;

typedef struct kefir_tree_node_iterator {
    struct kefir_tree_node *current;
    struct kefir_list pending;
} kefir_tree_node_iterator_t;

kefir_result_t kefir_tree_init(struct kefir_tree_node *, void *);
kefir_result_t kefir_tree_free(struct kefir_mem *, struct kefir_tree_node *);
kefir_result_t kefir_tree_on_removal(struct kefir_tree_node *, kefir_result_t (*)(struct kefir_mem *, void *, void *), void *);
kefir_result_t kefir_tree_insert_child(struct kefir_mem *, struct kefir_tree_node *, void *, struct kefir_tree_node **);
struct kefir_tree_node *kefir_tree_first_child(const struct kefir_tree_node *);
struct kefir_tree_node *kefir_tree_next_sibling(const struct kefir_tree_node *);
struct kefir_tree_node *kefir_tree_prev_sibling(const struct kefir_tree_node *);

kefir_result_t kefir_tree_iter(struct kefir_tree_node *, struct kefir_tree_node_iterator *);
kefir_result_t kefir_tree_iter_next(struct kefir_mem *, struct kefir_tree_node_iterator *);

#endif