#ifndef KEFIR_AST_TRANSLATOR_SCOPE_GLOBAL_SCOPE_LAYOUT_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_GLOBAL_SCOPE_LAYOUT_H_

#include "kefir/ast-translator/scope/scoped_identifier.h"
#include "kefir/ast-translator/type_resolver.h"

typedef struct kefir_ast_translator_global_scope_layout {
    struct kefir_ast_global_context *global_context;
    struct kefir_list external_objects;
    struct kefir_list external_thread_local_objects;
    struct kefir_list static_objects;
    struct kefir_list static_thread_local_objects;

    struct kefir_ir_type *static_layout;
    kefir_id_t static_layout_id;
    struct kefir_ir_type *static_thread_local_layout;
    kefir_id_t static_thread_local_layout_id;
} kefir_ast_translator_global_scope_layout_t;

kefir_result_t kefir_ast_translator_global_scope_layout_init(struct kefir_mem *, struct kefir_ir_module *,
                                                             struct kefir_ast_translator_global_scope_layout *);

kefir_result_t kefir_ast_translator_global_scope_layout_free(struct kefir_mem *,
                                                             struct kefir_ast_translator_global_scope_layout *);

kefir_result_t kefir_ast_translator_build_global_scope_layout(struct kefir_mem *, struct kefir_ir_module *,
                                                              struct kefir_ast_global_context *,
                                                              const struct kefir_ast_translator_environment *,
                                                              struct kefir_ast_translator_type_resolver *,
                                                              struct kefir_ast_translator_global_scope_layout *);

#endif
