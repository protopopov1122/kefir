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

#include <string.h>
#include "kefir/core/list.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_list_init(struct kefir_list *list) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected non-NULL list pointer"));
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    list->entry_removal.callback = NULL;
    list->entry_removal.data = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_list_free(struct kefir_mem *mem, struct kefir_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected non-NULL list pointer"));
    struct kefir_list_entry *current = list->head, *next = NULL;
    while (current != NULL) {
        next = current->next;
        if (list->entry_removal.callback != NULL) {
            REQUIRE_OK(list->entry_removal.callback(mem, list, current, list->entry_removal.data));
        }
        KEFIR_FREE(mem, current);
        current = next;
    }
    list->head = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_list_on_remove(struct kefir_list *list,
                                    kefir_result_t (*callback)(struct kefir_mem *, struct kefir_list *,
                                                               struct kefir_list_entry *, void *),
                                    void *data) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected non-NULL list pointer"));
    list->entry_removal.callback = callback;
    list->entry_removal.data = data;
    return KEFIR_OK;
}

kefir_size_t kefir_list_length(const struct kefir_list *list) {
    REQUIRE(list != NULL, 0);
    return list->length;
}

kefir_result_t kefir_list_insert_after(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *iter,
                                       void *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected non-NULL list pointer"));
    REQUIRE_ELSE((iter == NULL && list->head == NULL) || (iter != NULL && list->head != NULL),
                 KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL iterator for non-empty list"));
    struct kefir_list_entry *entry = KEFIR_MALLOC(mem, sizeof(struct kefir_list_entry));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate list entry"));
    entry->prev = NULL;
    entry->next = NULL;
    entry->value = value;
    if (iter == NULL) {
        list->head = entry;
        list->tail = entry;
    } else {
        entry->prev = iter;
        entry->next = iter->next;
        iter->next = entry;
        if (entry->next != NULL) {
            entry->next->prev = entry;
        }
        if (iter == list->tail) {
            list->tail = entry;
        }
    }
    list->length++;
    return KEFIR_OK;
}

kefir_result_t kefir_list_pop(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *iter) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected non-NULL list pointer"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected non-NULL list iterator"));
    if (list->entry_removal.callback != NULL) {
        REQUIRE_OK(list->entry_removal.callback(mem, list, iter, list->entry_removal.data));
    }
    if (iter == list->head) {
        list->head = iter->next;
    }
    if (iter == list->tail) {
        list->tail = iter->prev;
    }
    if (iter->next != NULL) {
        iter->next->prev = iter->prev;
    }
    if (iter->prev != NULL) {
        iter->prev->next = iter->next;
    }
    KEFIR_FREE(mem, iter);
    list->length--;
    return KEFIR_OK;
}

kefir_result_t kefir_list_clear(struct kefir_mem *mem, struct kefir_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected non-NULL list pointer"));
    while (list->head != NULL) {
        struct kefir_list_entry *entry = list->head;
        if (list->entry_removal.callback != NULL) {
            REQUIRE_OK(list->entry_removal.callback(mem, list, entry, list->entry_removal.data));
        }

        list->head = entry->next;
        if (list->head != NULL) {
            list->head->prev = NULL;
        }
        if (list->tail == entry) {
            list->tail = NULL;
        }
        list->length--;
        KEFIR_FREE(mem, entry);
    }
    return KEFIR_OK;
}

struct kefir_list_entry *kefir_list_head(const struct kefir_list *list) {
    REQUIRE(list != NULL, NULL);
    return list->head;
}

struct kefir_list_entry *kefir_list_tail(const struct kefir_list *list) {
    REQUIRE(list != NULL, NULL);
    return list->tail;
}

struct kefir_list_entry *kefir_list_at(const struct kefir_list *list, kefir_size_t index) {
    REQUIRE(list != NULL, NULL);
    struct kefir_list_entry *entry = list->head;
    while (index-- && entry != NULL) {
        entry = entry->next;
    }
    return entry;
}

void *kefir_list_next(const struct kefir_list_entry **current) {
    REQUIRE(current != NULL, NULL);
    if (*current == NULL) {
        return NULL;
    }
    void *value = (*current)->value;
    *current = (*current)->next;
    return value;
}

kefir_result_t kefir_list_move_all(struct kefir_list *dst, struct kefir_list *src) {
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid destination list"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source list"));

    struct kefir_list_entry *src_head = src->head;
    struct kefir_list_entry *src_tail = src->tail;
    kefir_size_t src_length = src->length;
    if (src_head != NULL) {
        src->head = NULL;
        src->tail = NULL;
        src->length = 0;

        if (dst->tail != NULL) {
            dst->tail->next = src_head;
            src_head->prev = dst->tail;
            dst->tail = src_tail;
        } else {
            dst->head = src_head;
            dst->tail = src_tail;
        }
        dst->length += src_length;
    }
    return KEFIR_OK;
}
