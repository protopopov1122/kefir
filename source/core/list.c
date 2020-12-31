#include <string.h>
#include "kefir/core/list.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_list_init(struct kefir_list *list, kefir_size_t el_size) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL list pointer"));
    list->element_size = el_size;
    list->head = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_list_free(struct kefir_mem *mem, struct kefir_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL list pointer"));
    struct kefir_list_entry *current = list->head, *next = NULL;
    while (current != NULL) {
        next = current->next;
        if (list->element_size > 0 && current->value != NULL) {
            KEFIR_FREE(mem, current->value);
        }
        KEFIR_FREE(mem, current);
        current = next;
    }
    list->head = NULL;
    return KEFIR_OK;
}

bool kefir_list_owning(const struct kefir_list *list) {
    REQUIRE(list != NULL, false);
    return list->element_size > 0;
}

kefir_size_t kefir_list_length(const struct kefir_list *list) {
    REQUIRE(list != NULL, 0);
    kefir_size_t length = 0;
    struct kefir_list_entry *current = list->head;
    while (current != NULL) {
        length++;
        current = current->next;
    }
    return length;
}

void *kefir_list_append(struct kefir_mem *mem, struct kefir_list *list, void *value) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(list != NULL, NULL);
    struct kefir_list_entry *entry = KEFIR_MALLOC(mem, sizeof(struct kefir_list_entry));
    REQUIRE(entry != NULL, NULL);
    entry->next = NULL;
    if (list->element_size == 0) {
        entry->value = value;
    } else {
        entry->value = KEFIR_MALLOC(mem, list->element_size);
        REQUIRE_ELSE(entry->value != NULL, {
            KEFIR_FREE(mem, entry);
            return NULL;
        });
        if (value != NULL) {
            memcpy(entry->value, value, list->element_size);
        }
    }
    if (list->head == NULL) {
        list->head = entry;
        return entry->value;
    }
    struct kefir_list_entry *current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = entry;
    return entry->value;
}

kefir_result_t kefir_list_pop_back(struct kefir_mem *mem, struct kefir_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "EXpected non-NULL list pointer"));
    if (list->head == NULL) {
        return KEFIR_NOT_FOUND;
    }
    struct kefir_list_entry *prev = list->head, *current = list->head;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }
    prev->next = NULL;
    if (list->element_size > 0) {
        KEFIR_FREE(mem, current->value);
    }
    KEFIR_FREE(mem, current);
    return KEFIR_OK;
}

void *kefir_list_iter(const struct kefir_list *list) {
    REQUIRE(list != NULL, NULL);
    return list->head;
}

void *kefir_list_next(void **iter) {
    struct kefir_list_entry **current = (struct kefir_list_entry **) iter;
    if (*current == NULL) {
        return NULL;
    }
    void *value = (*current)->value;
    *iter = (*current)->next;
    return value;
}