#ifndef KEFIR_CORE_LIST_H_
#define KEFIR_CORE_LIST_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef struct kefir_list_entry {
    void *value;
    struct kefir_list_entry *prev;
    struct kefir_list_entry *next;
} kefir_list_entry_t;

typedef struct kefir_list {
    struct kefir_list_entry *head;
    struct kefir_list_entry *tail;
    kefir_size_t length;
    struct {
        kefir_result_t (*callback)(struct kefir_mem *, struct kefir_list *, struct kefir_list_entry *, void *);
        void *data;
    } entry_removal;
} kefir_list_t;

kefir_result_t kefir_list_init(struct kefir_list *);
kefir_result_t kefir_list_free(struct kefir_mem *, struct kefir_list *);
kefir_result_t kefir_list_on_remove(struct kefir_list *,
                                kefir_result_t (*)(struct kefir_mem *, struct kefir_list *, struct kefir_list_entry *, void *),
                                void *);
kefir_size_t kefir_list_length(const struct kefir_list *);
kefir_result_t kefir_list_insert_after(struct kefir_mem *, struct kefir_list *, struct kefir_list_entry *, void *);
kefir_result_t kefir_list_pop(struct kefir_mem *, struct kefir_list *, struct kefir_list_entry *);
struct kefir_list_entry *kefir_list_head(const struct kefir_list *);
struct kefir_list_entry *kefir_list_tail(const struct kefir_list *);
struct kefir_list_entry *kefir_list_at(const struct kefir_list *, kefir_size_t);
void *kefir_list_next(const struct kefir_list_entry **);

#endif