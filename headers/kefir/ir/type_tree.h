#ifndef KEFIR_IR_TYPE_TREE_H_
#define KEFIR_IR_TYPE_TREE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ir/type.h"
#include "kefir/core/hashtree.h"
#include "kefir/core/list.h"

typedef struct kefir_ir_type_tree_node {
    const struct kefir_ir_type *type;
    kefir_size_t index;
    kefir_size_t relative_slot;
    kefir_size_t slot_width;

    const struct kefir_ir_type_tree_node *parent;
    struct kefir_list subtypes;
} kefir_ir_type_tree_node_t;

typedef struct kefir_ir_type_tree {
    const struct kefir_ir_type *type;
    struct kefir_list roots;
    struct kefir_hashtree index;
} kefir_ir_type_tree_t;

kefir_result_t kefir_ir_type_tree_init(struct kefir_mem *, const struct kefir_ir_type *, struct kefir_ir_type_tree *);
kefir_result_t kefir_ir_type_tree_free(struct kefir_mem *, struct kefir_ir_type_tree *);
kefir_result_t kefir_ir_type_tree_at(const struct kefir_ir_type_tree *, kefir_size_t,
                                     const struct kefir_ir_type_tree_node **);

#endif