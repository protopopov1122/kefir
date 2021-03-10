#ifndef KEFIR_AST_TRANSLATOR_SCOPE_LAYOUT_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_LAYOUT_H_

#include "kefir/ast/local_context.h"
#include "kefir/ast/translator/environment.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/ir/module.h"
#include "kefir/core/hashtree.h"

typedef struct kefir_ast_scoped_identifier_layout {
    kefir_id_t type_id;
    kefir_size_t type_index;
} kefir_ast_scoped_identifier_layout_t;

_Static_assert(sizeof(struct kefir_ast_scoped_identifier_layout) <= KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE,
    "Unable to fit scoped identifier layout into payload field");

typedef struct kefir_ast_global_scope_layout {
    struct kefir_hashtree external_objects;
    struct kefir_hashtree external_thread_local_objects;
    struct kefir_hashtree static_objects;
    struct kefir_hashtree static_thread_local_objects;

    struct kefir_ir_type *static_layout;
    kefir_id_t static_layout_id;
    struct kefir_ir_type *static_thread_local_layout;
    kefir_id_t static_thread_local_layout_id;
} kefir_ast_global_scope_layout_t;

typedef struct kefir_ast_local_scope_layout {
    struct kefir_ast_global_scope_layout *global;
    struct kefir_ir_type *local_layout;
    kefir_id_t local_layout_id;
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
kefir_result_t kefir_ast_lobal_scope_layout_free(struct kefir_mem *,
                                             struct kefir_ast_local_scope_layout *);

kefir_result_t kefir_ast_translate_global_scope_layout(struct kefir_mem *,
                                                   struct kefir_ir_module *,
                                                   const struct kefir_ast_global_context *,
                                                   const struct kefir_ast_translator_environment *,
                                                   struct kefir_ast_global_scope_layout *);
kefir_result_t kefir_ast_translate_local_scope_layout(struct kefir_mem *,
                                                  const struct kefir_ast_local_context *,
                                                  const struct kefir_ast_translator_environment *,
                                                  struct kefir_ast_local_scope_layout *);

#endif