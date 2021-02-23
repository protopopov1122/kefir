#ifndef KEFIR_AST_CONTEXT_H_
#define KEFIR_AST_CONTEXT_H_

#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"
#include "kefir/ast/scope.h"

typedef struct kefir_ast_global_context {
    struct kefir_symbol_table symbols;
    const struct kefir_ast_type_traits *type_traits;
    struct kefir_ast_type_storage type_storage;
    struct kefir_ast_identifier_flat_scope object_scope;
    struct kefir_hashtree external_object_declarations;
} kefir_ast_global_context_t;

typedef struct kefir_ast_context {
    struct kefir_ast_global_context *global;
    struct kefir_ast_identifier_block_scope local_object_scope;
} kefir_ast_context_t;

kefir_result_t kefir_ast_global_context_init(struct kefir_mem *,
                                                     const struct kefir_ast_type_traits *,
                                                     struct kefir_ast_global_context *);
kefir_result_t kefir_ast_global_context_free(struct kefir_mem *,
                                                     struct kefir_ast_global_context *);

kefir_result_t kefir_ast_context_init(struct kefir_mem *,
                                              struct kefir_ast_global_context *,
                                              struct kefir_ast_context *);
kefir_result_t kefir_ast_context_free(struct kefir_mem *,
                                              struct kefir_ast_context *);

kefir_result_t kefir_ast_context_resolve_object_identifier(const struct kefir_ast_context *,
                                                       const char *,
                                                       const struct kefir_ast_scoped_identifier **,
                                                       kefir_ast_scoped_identifier_linkage_t *);

kefir_result_t kefir_ast_context_push_block_scope(struct kefir_mem *,
                                                          struct kefir_ast_context *);
kefir_result_t kefir_ast_context_pop_block_scope(struct kefir_ast_context *);

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
kefir_result_t kefir_ast_context_declare_external(struct kefir_mem *,
                                              struct kefir_ast_context *,
                                              const char *,
                                              const struct kefir_ast_type *,
                                              struct kefir_ast_alignment *);
kefir_result_t kefir_ast_context_declare_external_thread_local(struct kefir_mem *,
                                                           struct kefir_ast_context *,
                                                           const char *,
                                                           const struct kefir_ast_type *,
                                                           struct kefir_ast_alignment *);
kefir_result_t kefir_ast_context_define_static(struct kefir_mem *,
                                           struct kefir_ast_context *,
                                           const char *,
                                           const struct kefir_ast_type *,
                                           struct kefir_ast_alignment *);
kefir_result_t kefir_ast_context_define_static_thread_local(struct kefir_mem *,
                                                          struct kefir_ast_context *,
                                                          const char *,
                                                          const struct kefir_ast_type *,
                                                          struct kefir_ast_alignment *);
kefir_result_t kefir_ast_context_define_auto(struct kefir_mem *,
                                         struct kefir_ast_context *,
                                         const char *,
                                         const struct kefir_ast_type *,
                                         struct kefir_ast_alignment *);
kefir_result_t kefir_ast_context_define_register(struct kefir_mem *,
                                             struct kefir_ast_context *,
                                             const char *,
                                             const struct kefir_ast_type *,
                                             struct kefir_ast_alignment *);

#endif