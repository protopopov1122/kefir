#ifndef KEFIR_AST_TRANSLATOR_GLOBAL_SCOPE_H_
#define KEFIR_AST_TRANSLATOR_GLOBAL_SCOPE_H_

#include "kefir/ast-translator/scope.h"

typedef struct kefir_ast_translator_global_scope {
    struct kefir_hashtree external_objects;
    struct kefir_hashtree external_thread_local_objects;
    struct kefir_hashtree static_objects;
    struct kefir_hashtree static_thread_local_objects;

    struct kefir_ir_type *static_layout;
    kefir_id_t static_layout_id;
    struct kefir_ir_type *static_thread_local_layout;
    kefir_id_t static_thread_local_layout_id;
} kefir_ast_translator_global_scope_t;

kefir_result_t kefir_ast_translator_global_scope_init(struct kefir_mem *,
                                                  struct kefir_ir_module *,
                                                  struct kefir_ast_translator_global_scope *);
kefir_result_t kefir_ast_translator_global_scope_free(struct kefir_mem *,
                                                  struct kefir_ast_translator_global_scope *);

kefir_result_t kefir_ast_translate_global_scope(struct kefir_mem *,
                                            struct kefir_ir_module *,
                                            const struct kefir_ast_global_context *,
                                            const struct kefir_ast_translator_environment *,
                                            struct kefir_ast_translator_global_scope *);

#endif