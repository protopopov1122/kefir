#ifndef KEFIR_PREPROCESSOR_PREDEFINED_MACRO_H_
#define KEFIR_PREPROCESSOR_PREDEFINED_MACRO_H_

#include "kefir/preprocessor/macro.h"

typedef struct kefir_preprocessor kefir_preprocessor_t;

typedef struct kefir_preprocessor_predefined_macro_scope {
    struct kefir_preprocessor_macro_scope scope;
    struct kefir_preprocessor *preprocessor;
} kefir_preprocessor_predefined_macro_scope_t;

kefir_result_t kefir_preprocessor_predefined_macro_scope_init(struct kefir_preprocessor_predefined_macro_scope *,
                                                              struct kefir_preprocessor *);

#endif
