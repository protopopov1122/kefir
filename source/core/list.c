#include <string.h>
#include "kefir/core/list.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_list_init(struct kefir_list *list) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL list pointer"));
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    list->entry_removal.callback = NULL;
    list->entry_removal.data = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_list_free(struct kefir_mem *mem, struct kefir_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL list pointer"));
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
                                kefir_result_t (*callback)(struct kefir_mem *, struct kefir_list *, struct kefir_list_entry *, void *),
                                void *data) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL list pointer"));
    list->entry_removal.callback = callback;
    list->entry_removal.data = data;
    return KEFIR_OK;
}

kefir_size_t kefir_list_length(const struct kefir_list *list) {
    REQUIRE(list != NULL, 0);
    return list->length;
}

kefir_result_t kefir_list_insert_after(struct kefir_mem *mem,
                                   struct kefir_list *list,
                                   struct kefir_list_entry *iter,
                                   void *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL list pointer"));
    REQUIRE_ELSE((iter == NULL && list->head == NULL) ||
        (iter != NULL && list->head != NULL),
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
    return KEFIR_OK;
}

kefir_result_t kefir_list_pop(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *iter) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL list pointer"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL list iterator"));
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

void *kefir_list_next(const struct kefir_list_entry **current) {
    REQUIRE(current != NULL, NULL);
    if (*current == NULL) {
        return NULL;
    }
    void *value = (*current)->value;
    *current = (*current)->next;
    return value;
}