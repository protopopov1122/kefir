#ifndef KEFIR_AST_TRANSLATOR_LOCAL_SCOPE_H_
#define KEFIR_AST_TRANSLATOR_LOCAL_SCOPE_H_

#include "kefir/ast-translator/scope.h"
#include "kefir/ast-translator/global_scope.h"

typedef struct kefir_ast_translator_local_scope {
    struct kefir_ast_translator_global_scope *global;

    struct kefir_hashtree static_objects;
    struct kefir_hashtree static_thread_local_objects;
    struct kefir_hashtree local_objects;

    struct kefir_ir_type *local_layout;
    kefir_id_t local_layout_id;
} kefir_ast_translator_local_scope_t;

kefir_result_t kefir_ast_translator_local_scope_init(struct kefir_mem *,
                                                 struct kefir_ir_module *,
                                                 struct kefir_ast_translator_global_scope *,
                                                 struct kefir_ast_translator_local_scope *);

kefir_result_t kefir_ast_translator_local_scope_free(struct kefir_mem *,
                                                 struct kefir_ast_translator_local_scope *);

kefir_result_t kefir_ast_translate_local_scope(struct kefir_mem *,
                                           const struct kefir_ast_local_context *,
                                           const struct kefir_ast_translator_environment *,
                                           struct kefir_ast_translator_local_scope *);

#endif