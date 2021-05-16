#ifndef KEFIR_CORE_ERROR_H_
#define KEFIR_CORE_ERROR_H_

#include "kefir/core/basic-types.h"

typedef struct kefir_error {
    kefir_result_t code;
    const char *message;
    const char *file;
    unsigned int line;
} kefir_error_t;

const struct kefir_error *kefir_current_error();
void kefir_clear_error();
kefir_result_t kefir_set_error(kefir_result_t, const char *, const char *, unsigned int);

#define KEFIR_SET_ERROR(code, message) \
    kefir_set_error((code), (message), __FILE__, __LINE__)

#endif
