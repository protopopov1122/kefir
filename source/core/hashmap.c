#include <string.h>
#include "kefir/core/hashmap.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"

static kefir_uint64_t str_hash(const char *str) {
    REQUIRE(str != NULL, 0);
    kefir_uint64_t hash = 7;
    for (kefir_size_t i = 0; i < strlen(str); i++) {
        hash = hash * 31 + str[i];
    }
    return hash;
}

kefir_result_t kefir_hashmap_init(struct kefir_hashmap *hash, void *area, kefir_size_t capacity) {
    REQUIRE(hash != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash map pointer"));
    REQUIRE((area != NULL && capacity > 0) || (area == NULL && capacity == 0),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash map data pointer for non-zero capacity"));
    hash->entries = (struct kefir_hashmap_entry *) area;
    hash->size = 0;
    hash->capacity = capacity;
    memset(hash->entries, 0, sizeof(struct kefir_hashmap_entry) * capacity);
    return KEFIR_OK;
}

kefir_size_t kefir_hashmap_size(const struct kefir_hashmap *hash) {
    REQUIRE(hash != NULL, 0);
    return hash->size;
}

kefir_size_t kefir_hashmap_capacity(const struct kefir_hashmap *hash) {
    REQUIRE(hash != NULL, 0);
    return hash->capacity;
}

struct kefir_hashmap_entry *kefir_hashmap_at(const struct kefir_hashmap *hash, const char *key) {
    REQUIRE(hash != NULL, NULL);
    REQUIRE(key != NULL, NULL);
    kefir_uint64_t hashcode = str_hash(key);
    kefir_size_t index = hashcode % hash->capacity;
    if (hash->entries[index].key != NULL && strcmp(hash->entries[index].key, key) == 0) {
        return &hash->entries[index];
    }
    kefir_size_t count = 0;
    do {
        count++;
        index = (index + 1) % hash->capacity;
        if (hash->entries[index].key != NULL && strcmp(hash->entries[index].key, key) == 0) {
            return &hash->entries[index];
        }
    } while (count < hash->capacity);
    return NULL;
}

kefir_result_t kefir_hashmap_insert(struct kefir_hashmap *hash, const char *key, void *value) {
    REQUIRE(hash != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash map pointer"));
    REQUIRE(hash->size < hash->capacity, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Hash map is full"));
    struct kefir_hashmap_entry *entry = kefir_hashmap_at(hash, key);
    if (entry != NULL) {
        entry->value = value;
        return KEFIR_OK;
    }
    kefir_uint64_t hashcode = str_hash(key);
    kefir_size_t index = hashcode % hash->capacity;
    kefir_size_t count = 0;
    while (count < hash->capacity) {
        if (hash->entries[index].key == NULL) {
            hash->entries[index].hash = hashcode;
            hash->entries[index].key = key;
            hash->entries[index].value = value;
            hash->size++;
            return KEFIR_OK;
        }
        count++;
        index = (index + 1) % hash->capacity;
    }
    return KEFIR_INTERNAL_ERROR;
}

kefir_result_t kefir_hashmap_remove(struct kefir_hashmap *hash, const char *key) {
    REQUIRE(hash != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash map pointer"));
    struct kefir_hashmap_entry *entry = kefir_hashmap_at(hash, key);
    if (entry != NULL) {
        entry->hash = 0;
        entry->key = NULL;
        entry->value = NULL;
        return KEFIR_OK;
    } else {
        return KEFIR_NOT_FOUND;
    }
}

kefir_result_t kefir_hashmap_alloc(struct kefir_mem *mem, struct kefir_hashmap *hash, kefir_size_t capacity) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(hash != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash map pointer"));
    if (capacity == 0) {
        return kefir_hashmap_init(hash, NULL, 0);
    }
    void *area = KEFIR_MALLOC(mem, sizeof(struct kefir_hashmap_entry) * capacity);
    REQUIRE(area != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocated memory for hash map"));
    kefir_result_t res = kefir_hashmap_init(hash, area, capacity);
    if (res != KEFIR_OK) {
        KEFIR_FREE(mem, area);
    }
    return res;
}

kefir_result_t kefir_hashmap_free(struct kefir_mem *mem, struct kefir_hashmap *hash) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(hash != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid hash map pointer"));
    if (hash->entries != NULL) {
        KEFIR_FREE(mem, hash->entries);
        hash->entries = NULL;
        hash->capacity = 0;
        hash->size = 0;
    }
    return KEFIR_OK;
}