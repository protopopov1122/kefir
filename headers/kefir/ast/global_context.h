#ifndef KEFIR_AST_GLOBAL_CONTEXT_H_
#define KEFIR_AST_GLOBAL_CONTEXT_H_

#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"
#include "kefir/ast/scope.h"

typedef struct kefir_ast_global_context {
    struct kefir_symbol_table symbols;
    const struct kefir_ast_type_traits *type_traits;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_identifier_flat_scope ordinary_scope;
    struct kefir_ast_identifier_flat_scope tag_scope;
    struct kefir_hashtree external_object_declarations;
    struct kefir_hashtree external_function_declarations;
} kefir_ast_global_context_t;

kefir_result_t kefir_ast_global_context_init(struct kefir_mem *,
                                         const struct kefir_ast_type_traits *,
                                         struct kefir_ast_global_context *);
kefir_result_t kefir_ast_global_context_free(struct kefir_mem *,
                                         struct kefir_ast_global_context *);

kefir_result_t kefir_ast_global_context_declare_external(struct kefir_mem *,
                                                     struct kefir_ast_global_context *,
                                                     const char *,
                                                     const struct kefir_ast_type *,
                                                     struct kefir_ast_alignment *);
kefir_result_t kefir_ast_global_context_declare_external_thread_local(struct kefir_mem *,
                                                                  struct kefir_ast_global_context *,
                                                                  const char *,
                                                                  const struct kefir_ast_type *,
                                                                  struct kefir_ast_alignment *);
kefir_result_t kefir_ast_global_context_define_external(struct kefir_mem *,
                                                    struct kefir_ast_global_context *,
                                                    const char *,
                                                    const struct kefir_ast_type *,
                                                    struct kefir_ast_alignment *);
kefir_result_t kefir_ast_global_context_define_external_thread_local(struct kefir_mem *,
                                                                 struct kefir_ast_global_context *,
                                                                 const char *,
                                                                 const struct kefir_ast_type *,
                                                                  struct kefir_ast_alignment *);
kefir_result_t kefir_ast_global_context_define_static(struct kefir_mem *,
                                                  struct kefir_ast_global_context *,
                                                  const char *,
                                                  const struct kefir_ast_type *,
                                                  struct kefir_ast_alignment *);
kefir_result_t kefir_ast_global_context_define_static_thread_local(struct kefir_mem *,
                                                               struct kefir_ast_global_context *,
                                                               const char *,
                                                               const struct kefir_ast_type *,
                                                               struct kefir_ast_alignment *);
kefir_result_t kefir_ast_global_context_define_constant(struct kefir_mem *,
                                                    struct kefir_ast_global_context *,
                                                    const char *,
                                                    struct kefir_ast_constant_expression *);
kefir_result_t kefir_ast_global_context_define_tag(struct kefir_mem *,
                                               struct kefir_ast_global_context *,
                                               const struct kefir_ast_type *);
kefir_result_t kefir_ast_global_context_define_type(struct kefir_mem *,
                                                struct kefir_ast_global_context *,
                                                const char *,
                                                const struct kefir_ast_type *);
kefir_result_t kefir_ast_global_context_declare_function(struct kefir_mem *,
                                                     struct kefir_ast_global_context *,
                                                     kefir_ast_function_specifier_t,
                                                     const struct kefir_ast_type *);
kefir_result_t kefir_ast_global_context_define_function(struct kefir_mem *,
                                                     struct kefir_ast_global_context *,
                                                     kefir_ast_function_specifier_t,
                                                     const struct kefir_ast_type *);
kefir_result_t kefir_ast_global_context_define_static_function(struct kefir_mem *,
                                                           struct kefir_ast_global_context *,
                                                           kefir_ast_function_specifier_t,
                                                           const struct kefir_ast_type *);

#endif