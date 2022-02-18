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

#ifndef KEFIR_CORE_BLOCK_TREE_H_
#define KEFIR_CORE_BLOCK_TREE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/hashtree.h"

typedef struct kefir_block_tree kefir_block_tree_t;

typedef kefir_result_t (*kefir_block_tree_callback_t)(struct kefir_mem *, struct kefir_block_tree *, kefir_size_t,
                                                      void *, void *);

typedef struct kefir_block_tree_event {
    kefir_block_tree_callback_t callback;
    void *payload;
} kefir_block_tree_event_t;

typedef struct kefir_block_tree {
    struct kefir_hashtree tree;
    kefir_size_t block_size;

    struct kefir_block_tree_event on_block_init;
    struct kefir_block_tree_event on_block_removal;
} kefir_block_tree_t;

typedef struct kefir_block_tree_iterator {
    struct kefir_hashtree_node_iterator tree_iter;
    kefir_size_t block_id;
    void *block;
} kefir_block_tree_iterator_t;

kefir_result_t kefir_block_tree_init(struct kefir_block_tree *, kefir_size_t);
kefir_result_t kefir_block_tree_free(struct kefir_mem *, struct kefir_block_tree *);

kefir_result_t kefir_block_tree_on_block_init(struct kefir_block_tree *, kefir_block_tree_callback_t, void *);
kefir_result_t kefir_block_tree_on_block_removal(struct kefir_block_tree *, kefir_block_tree_callback_t, void *);

kefir_result_t kefir_block_tree_get_block_offset(const struct kefir_block_tree *, kefir_size_t, kefir_size_t *,
                                                 kefir_size_t *);
kefir_result_t kefir_block_tree_get_block(const struct kefir_block_tree *, kefir_size_t, void **);
kefir_result_t kefir_block_tree_block(struct kefir_mem *, struct kefir_block_tree *, kefir_size_t, void **);

kefir_result_t kefir_block_tree_iter(const struct kefir_block_tree *, struct kefir_block_tree_iterator *);
kefir_result_t kefir_block_tree_next(struct kefir_block_tree_iterator *);
kefir_result_t kefir_block_tree_iter_skip_to(const struct kefir_block_tree *, struct kefir_block_tree_iterator *,
                                             kefir_size_t);

#endif
