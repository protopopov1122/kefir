#include "kefir/core/error.h"

static _Thread_local struct kefir_error current_error;

const struct kefir_error *kefir_current_error() {
    if (current_error.code != KEFIR_OK) {
        return &current_error;
    } else {
        return NULL;
    }
}

void kefir_clear_error() {
    current_error.code = KEFIR_OK;
}

kefir_result_t kefir_set_error(kefir_result_t code, const char *message, const char *file, unsigned int line) {
    current_error.code = code;
    current_error.message = message;
    current_error.file = file;
    current_error.line = line;
    return code;
}
