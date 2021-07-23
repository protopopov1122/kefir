/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
kefir_result_t kefir_tree_on_removal(struct kefir_tree_node *, kefir_result_t (*)(struct kefir_mem *, void *, void *),
                                     void *);
kefir_result_t kefir_tree_insert_child(struct kefir_mem *, struct kefir_tree_node *, void *, struct kefir_tree_node **);
struct kefir_tree_node *kefir_tree_first_child(const struct kefir_tree_node *);
struct kefir_tree_node *kefir_tree_next_sibling(const struct kefir_tree_node *);
struct kefir_tree_node *kefir_tree_prev_sibling(const struct kefir_tree_node *);

kefir_result_t kefir_tree_iter(struct kefir_tree_node *, struct kefir_tree_node_iterator *);
kefir_result_t kefir_tree_iter_next(struct kefir_mem *, struct kefir_tree_node_iterator *);
kefir_result_t kefir_tree_iter_free(struct kefir_mem *, struct kefir_tree_node_iterator *);

#endif
