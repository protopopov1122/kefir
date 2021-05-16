#include <stdlib.h>
#include "kefir/core/mem.h"
#include "kefir/core/util.h"

kefir_result_t kefir_int_test(struct kefir_mem *);

static void *kefir_malloc(struct kefir_mem *mem, kefir_size_t sz) {
    UNUSED(mem);
    return malloc(sz);
}

static void *kefir_calloc(struct kefir_mem *mem, kefir_size_t num, kefir_size_t sz) {
    UNUSED(mem);
    return calloc(num, sz);
}

static void *kefir_realloc(struct kefir_mem *mem, void *ptr, kefir_size_t sz) {
    UNUSED(mem);
    return realloc(ptr, sz);
}

static void kefir_free(struct kefir_mem *mem, void *ptr) {
    UNUSED(mem);
    free(ptr);
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    struct kefir_mem mem = {
        .malloc = kefir_malloc,
        .calloc = kefir_calloc,
        .realloc = kefir_realloc,
        .free = kefir_free,
        .data = NULL
    };
    if (kefir_int_test(&mem) != KEFIR_OK) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
