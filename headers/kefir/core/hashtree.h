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

#ifndef KEFIR_CORE_HASHTREE_H_
#define KEFIR_CORE_HASHTREE_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef kefir_uint64_t kefir_hashtree_hash_t;
typedef kefir_uptr_t kefir_hashtree_key_t;
typedef kefir_uptr_t kefir_hashtree_value_t;

typedef struct kefir_hashtree_node {
    kefir_uptr_t key;
    kefir_hashtree_hash_t hash;
    kefir_hashtree_value_t value;

    kefir_size_t height;
    struct kefir_hashtree_node *parent;
    struct kefir_hashtree_node *left_child;
    struct kefir_hashtree_node *right_child;
} kefir_hashtree_node_t;

typedef struct kefir_hashtree {
    const struct kefir_hashtree_ops *ops;
    struct kefir_hashtree_node *root;
    struct {
        kefir_result_t (*callback)(struct kefir_mem *, struct kefir_hashtree *, kefir_hashtree_key_t,
                                   kefir_hashtree_value_t, void *);
        void *data;
    } node_remove;
} kefir_hashtree_t;

typedef struct kefir_hashtree_node_iterator {
    const struct kefir_hashtree_node *node;
    kefir_hashtree_hash_t last_hash;
} kefir_hashtree_node_iterator_t;

typedef struct kefir_hashtree_ops {
    kefir_hashtree_hash_t (*hash)(kefir_hashtree_key_t, void *);
    bool (*compare_keys)(kefir_hashtree_key_t, kefir_hashtree_key_t, void *);
    void *data;
} kefir_hashtree_ops_t;

typedef kefir_result_t (*kefir_hashtree_traversal_t)(const struct kefir_hashtree *, kefir_hashtree_key_t,
                                                     kefir_hashtree_value_t, void *);

kefir_result_t kefir_hashtree_init(struct kefir_hashtree *, const struct kefir_hashtree_ops *);
kefir_result_t kefir_hashtree_on_removal(struct kefir_hashtree *,
                                         kefir_result_t (*)(struct kefir_mem *, struct kefir_hashtree *,
                                                            kefir_hashtree_key_t, kefir_hashtree_value_t, void *),
                                         void *);
kefir_result_t kefir_hashtree_free(struct kefir_mem *, struct kefir_hashtree *);
kefir_result_t kefir_hashtree_insert(struct kefir_mem *, struct kefir_hashtree *, kefir_hashtree_key_t,
                                     kefir_hashtree_value_t);
kefir_result_t kefir_hashtree_at(const struct kefir_hashtree *, kefir_hashtree_key_t, struct kefir_hashtree_node **);
kefir_bool_t kefir_hashtree_has(const struct kefir_hashtree *, kefir_hashtree_key_t);
kefir_bool_t kefir_hashtree_empty(const struct kefir_hashtree *);
kefir_result_t kefir_hashtree_delete(struct kefir_mem *, struct kefir_hashtree *, kefir_hashtree_key_t);
kefir_result_t kefir_hashtree_clean(struct kefir_mem *, struct kefir_hashtree *);
const struct kefir_hashtree_node *kefir_hashtree_iter(const struct kefir_hashtree *,
                                                      struct kefir_hashtree_node_iterator *);
const struct kefir_hashtree_node *kefir_hashtree_next(struct kefir_hashtree_node_iterator *);

extern const struct kefir_hashtree_ops kefir_hashtree_str_ops;
extern const struct kefir_hashtree_ops kefir_hashtree_uint_ops;

#endif
