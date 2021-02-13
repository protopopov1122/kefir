#ifndef KEFIR_AST_TRANSLATOR_SCOPE_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_H_

#include "kefir/ast/context.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/hashtree.h"

typedef struct kefir_ast_global_scope_layout {
    struct kefir_hashtree objects;
    struct kefir_hashtree thread_local_objects;
    struct kefir_ir_type *static_objects;
    struct kefir_ir_type *thread_local_static_objects;
} kefir_ast_global_scope_layout_t;

typedef struct kefir_ast_local_scope_layout {
    struct kefir_ast_global_scope_layout *global;
    struct kefir_ir_type *locals;
} kefir_ast_local_scope_layout_t;

kefir_result_t kefir_ast_global_scope_layout_init(struct kefir_mem *,
                                              struct kefir_ir_module *,
                                              struct kefir_ast_global_scope_layout *);
kefir_result_t kefir_ast_local_scope_layout_init(struct kefir_mem *,
                                             struct kefir_ir_module *,
                                             struct kefir_ast_global_scope_layout *,
                                             struct kefir_ast_local_scope_layout *);
kefir_result_t kefir_ast_global_scope_layout_free(struct kefir_mem *,
                                              struct kefir_ast_global_scope_layout *);
kefir_result_t kefir_ast_lobal_scope_layout_free(struct kefir_ast_local_scope_layout *);

kefir_result_t kefir_ast_translate_global_scope_layout(struct kefir_mem *,
                                                   const struct kefir_ast_global_context *,
                                                   struct kefir_ast_global_scope_layout *);
kefir_result_t kefir_ast_translate_local_scope_layout(struct kefir_mem *,
                                                  const struct kefir_ast_context *,
                                                  struct kefir_ast_local_scope_layout *);

#endif