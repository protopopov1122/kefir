#ifndef KEFIR_AST_TRANSLATOR_SCOPE_LOCAL_SCOPE_LAYOUT_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_LOCAL_SCOPE_LAYOUT_H_

#include "kefir/ast-translator/scope/scoped_identifier.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/core/list.h"

typedef struct kefir_ast_translator_local_scope_layout {
    struct kefir_ast_translator_global_scope_layout *global;
    struct kefir_ir_type *local_layout;
    kefir_id_t local_layout_id;
    struct kefir_ast_type_layout *local_type_layout;

    struct kefir_list local_objects;
    struct kefir_list static_objects;
    struct kefir_list static_thread_local_objects;
} kefir_ast_translator_local_scope_layout_t;

kefir_result_t kefir_ast_translator_local_scope_layout_init(struct kefir_mem *, struct kefir_ir_module *,
                                                            struct kefir_ast_translator_global_scope_layout *,
                                                            struct kefir_ast_translator_local_scope_layout *);

kefir_result_t kefir_ast_translator_local_scope_layout_free(struct kefir_mem *,
                                                            struct kefir_ast_translator_local_scope_layout *);

kefir_result_t kefir_ast_translator_build_local_scope_layout(struct kefir_mem *, const struct kefir_ast_local_context *,
                                                             const struct kefir_ast_translator_environment *,
                                                             struct kefir_ir_module *,
                                                             struct kefir_ast_translator_type_resolver *,
                                                             struct kefir_ast_translator_local_scope_layout *);

#endif
