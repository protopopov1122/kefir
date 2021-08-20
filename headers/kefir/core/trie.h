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

#ifndef KEFIR_CORE_TRIE_H_
#define KEFIR_CORE_TRIE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/list.h"

typedef kefir_int64_t kefir_trie_key_t;
typedef kefir_uptr_t kefir_trie_value_t;

typedef struct kefir_trie {
    kefir_trie_value_t value;
    struct kefir_list vertices;
} kefir_trie_t;

typedef struct kefir_trie_vertex {
    kefir_trie_key_t key;
    struct kefir_trie node;
} kefir_trie_vertex_t;

kefir_result_t kefir_trie_init(struct kefir_trie *, kefir_trie_value_t);
kefir_result_t kefir_trie_free(struct kefir_mem *, struct kefir_trie *);
kefir_result_t kefir_trie_insert_vertex(struct kefir_mem *, struct kefir_trie *, kefir_trie_key_t, kefir_trie_value_t,
                                        struct kefir_trie_vertex **);
kefir_result_t kefir_trie_at(const struct kefir_trie *, kefir_trie_key_t, struct kefir_trie_vertex **);

#endif
