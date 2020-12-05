#ifndef KEFIR_CORE_MEM_H_
#define KEFIR_CORE_MEM_H_

#include "kefir/core/basic-types.h"

typedef struct kefir_mem {
    void *(*malloc)(struct kefir_mem *, kefir_size_t);
    void *(*calloc)(struct kefir_mem *, kefir_size_t, kefir_size_t);
    void *(*realloc)(struct kefir_mem *, void *, kefir_size_t);
    void (*free)(struct kefir_mem *, void *);
    void *data;
} kefir_mem_t;

#define KEFIR_MALLOC(mem, size) ((mem)->malloc((mem), (size)))
#define KEFIR_CALLOC(mem, num, size) ((mem)->calloc((mem), (num), (size)))
#define KEFIR_REALLOC(mem, ptr, size) ((mem)->realloc((mem), (ptr), (size)))
#define KEFIR_FREE(mem, ptr) ((mem)->free((mem), (ptr)))

#endif