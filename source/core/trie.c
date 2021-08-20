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

#include "kefir/core/trie.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t free_vertex(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                  void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_trie_vertex *, vertex, entry->value);
    REQUIRE_OK(kefir_list_free(mem, &vertex->node.vertices));
    KEFIR_FREE(mem, vertex);
    return KEFIR_OK;
}

kefir_result_t kefir_trie_init(struct kefir_trie *trie, kefir_trie_value_t value) {
    REQUIRE(trie != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid trie"));

    trie->value = value;
    REQUIRE_OK(kefir_list_init(&trie->vertices));
    REQUIRE_OK(kefir_list_on_remove(&trie->vertices, free_vertex, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_trie_free(struct kefir_mem *mem, struct kefir_trie *trie) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(trie != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid trie"));

    REQUIRE_OK(kefir_list_free(mem, &trie->vertices));
    return KEFIR_OK;
}

kefir_result_t kefir_trie_insert_vertex(struct kefir_mem *mem, struct kefir_trie *trie, kefir_trie_key_t key,
                                        kefir_trie_value_t value, struct kefir_trie_vertex **vertex_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(trie != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid trie"));

    for (const struct kefir_list_entry *iter = kefir_list_head(&trie->vertices); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_trie_vertex *, vertex, iter->value);
        REQUIRE(vertex->key != key,
                KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Vertex with provided key already exists in the trie"));
    }

    struct kefir_trie_vertex *vertex = KEFIR_MALLOC(mem, sizeof(struct kefir_trie_vertex));
    REQUIRE(vertex != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate trie vertex"));
    vertex->key = key;
    vertex->node.value = value;
    kefir_result_t res = kefir_list_init(&vertex->node.vertices);
    REQUIRE_CHAIN(&res, kefir_list_on_remove(&vertex->node.vertices, free_vertex, NULL));
    REQUIRE_CHAIN(&res, kefir_list_insert_after(mem, &trie->vertices, kefir_list_tail(&trie->vertices), vertex));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, vertex);
        return res;
    });
    ASSIGN_PTR(vertex_ptr, vertex);
    return KEFIR_OK;
}

kefir_result_t kefir_trie_at(const struct kefir_trie *trie, kefir_trie_key_t key,
                             struct kefir_trie_vertex **vertex_ptr) {
    REQUIRE(trie != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid trie"));
    REQUIRE(vertex_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to trie vertex"));

    *vertex_ptr = NULL;
    for (const struct kefir_list_entry *iter = kefir_list_head(&trie->vertices); iter != NULL && *vertex_ptr == NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_trie_vertex *, vertex, iter->value);
        if (vertex->key == key) {
            *vertex_ptr = vertex;
        }
    }
    REQUIRE(*vertex_ptr != NULL, KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Cannot find trie vertex with specified key"));
    return KEFIR_OK;
}
