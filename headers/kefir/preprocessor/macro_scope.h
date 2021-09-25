#ifndef KEFIR_PREPROCESSOR_MACRO_SCOPE_H_
#define KEFIR_PREPROCESSOR_MACRO_SCOPE_H_

#include "kefir/preprocessor/macro.h"

typedef struct kefir_preprocessor_overlay_macro_scope {
    struct kefir_preprocessor_macro_scope scope;

    const struct kefir_preprocessor_macro_scope *base;
    const struct kefir_preprocessor_macro_scope *overlay;
} kefir_preprocessor_overlay_macro_scope_t;

kefir_result_t kefir_preprocessor_overlay_macro_scope_init(struct kefir_preprocessor_overlay_macro_scope *,
                                                           const struct kefir_preprocessor_macro_scope *,
                                                           const struct kefir_preprocessor_macro_scope *);

#endif
