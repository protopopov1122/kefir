#ifndef KEFIR_CORE_HASHMAP_H_
#define KEFIR_CORE_HASHMAP_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef struct kefir_hashmap_entry {
    kefir_uint64_t hash;
    const char *key;
    void *value;
} kefir_hashmap_entry_t;

typedef struct kefir_hashmap {
    struct kefir_hashmap_entry *entries;
    kefir_size_t size;
    kefir_size_t capacity;
} kefir_hashmap_t;

kefir_result_t kefir_hashmap_init(struct kefir_hashmap *, void *, kefir_size_t);
kefir_size_t kefir_hashmap_size(const struct kefir_hashmap *);
kefir_size_t kefir_hashmap_capacity(const struct kefir_hashmap *);
struct kefir_hashmap_entry *kefir_hashmap_at(const struct kefir_hashmap *, const char *);
kefir_result_t kefir_hashmap_insert(struct kefir_hashmap *, const char *, void *);
kefir_result_t kefir_hashmap_remove(struct kefir_hashmap *, const char *);
kefir_result_t kefir_hashmap_alloc(struct kefir_mem *, struct kefir_hashmap *, kefir_size_t);
kefir_result_t kefir_hashmap_free(struct kefir_mem *, struct kefir_hashmap *);

#endif