#ifndef KEFIR_PREPROCESSOR_USER_MACRO_H_
#define KEFIR_PREPROCESSOR_USER_MACRO_H_

#include "kefir/preprocessor/macro.h"

typedef struct kefir_preprocessor_user_macro {
    struct kefir_preprocessor_macro macro;
    struct kefir_list parameters;
    kefir_bool_t vararg;
    struct kefir_token_buffer replacement;
} kefir_preprocessor_user_macro_t;

struct kefir_preprocessor_user_macro *kefir_preprocessor_user_macro_new_object(struct kefir_mem *,
                                                                               struct kefir_symbol_table *,
                                                                               const char *);
struct kefir_preprocessor_user_macro *kefir_preprocessor_user_macro_new_function(struct kefir_mem *,
                                                                                 struct kefir_symbol_table *,
                                                                                 const char *);
kefir_result_t kefir_preprocessor_user_macro_free(struct kefir_mem *, struct kefir_preprocessor_user_macro *);

typedef struct kefir_preprocessor_user_macro_scope {
    struct kefir_preprocessor_macro_scope scope;
    const struct kefir_preprocessor_user_macro_scope *parent;
    struct kefir_hashtree macros;
} kefir_preprocessor_user_macro_scope_t;

kefir_result_t kefir_preprocessor_user_macro_scope_init(const struct kefir_preprocessor_user_macro_scope *,
                                                        struct kefir_preprocessor_user_macro_scope *);
kefir_result_t kefir_preprocessor_user_macro_scope_free(struct kefir_mem *,
                                                        struct kefir_preprocessor_user_macro_scope *);
kefir_result_t kefir_preprocessor_user_macro_scope_insert(struct kefir_mem *,
                                                          struct kefir_preprocessor_user_macro_scope *,
                                                          struct kefir_preprocessor_user_macro *);
kefir_result_t kefir_preprocessor_user_macro_scope_at(const struct kefir_preprocessor_user_macro_scope *, const char *,
                                                      const struct kefir_preprocessor_user_macro **);
kefir_result_t kefir_preprocessor_user_macro_scope_remove(struct kefir_mem *,
                                                          struct kefir_preprocessor_user_macro_scope *, const char *);

#endif
