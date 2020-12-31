#ifndef KEFIR_CORE_LIST_H_
#define KEFIR_CORE_LIST_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef struct kefir_list_entry {
    void *value;
    struct kefir_list_entry *next;
} kefir_list_entry_t;

typedef struct kefir_list {
    kefir_size_t element_size;
    struct kefir_list_entry *head;
} kefir_list_t;

kefir_result_t kefir_list_init(struct kefir_list *, kefir_size_t);
kefir_result_t kefir_list_free(struct kefir_mem *, struct kefir_list *);
bool kefir_list_owning(const struct kefir_list *);
kefir_size_t kefir_list_length(const struct kefir_list *);
void *kefir_list_append(struct kefir_mem *, struct kefir_list *, void *);
kefir_result_t kefir_list_pop_back(struct kefir_mem *, struct kefir_list *);
void *kefir_list_iter(const struct kefir_list *);
void *kefir_list_next(void **);

#endif