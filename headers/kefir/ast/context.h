#ifndef KEFIR_AST_CONTEXT_H_
#define KEFIR_AST_CONTEXT_H_

#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"
#include "kefir/ast/scope.h"
#include "kefir/ast/global_context.h"

typedef struct kefir_ast_context {
    struct kefir_ast_global_context *global;
    struct kefir_ast_identifier_block_scope local_ordinary_scope;
    struct kefir_ast_identifier_block_scope local_tag_scope;
} kefir_ast_context_t;

kefir_result_t kefir_ast_context_init(struct kefir_mem *,
                                  struct kefir_ast_global_context *,
                                  struct kefir_ast_context *);
kefir_result_t kefir_ast_context_free(struct kefir_mem *,
                                  struct kefir_ast_context *);

kefir_result_t kefir_ast_context_resolve_object_identifier(const struct kefir_ast_context *,
                                                       const char *,
                                                       const struct kefir_ast_scoped_identifier **,
                                                       kefir_ast_scoped_identifier_linkage_t *);

kefir_result_t kefir_ast_context_resolve_scoped_ordinary_identifier(const struct kefir_ast_context *,
                                                                const char *,
                                                                const struct kefir_ast_scoped_identifier **);

kefir_result_t kefir_ast_context_resolve_scoped_tag_identifier(const struct kefir_ast_context *,
                                                           const char *,
                                                           const struct kefir_ast_scoped_identifier **);

kefir_result_t kefir_ast_context_push_block_scope(struct kefir_mem *,
                                                          struct kefir_ast_context *);
kefir_result_t kefir_ast_context_pop_block_scope(struct kefir_ast_context *);
                                            
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
kefir_result_t kefir_ast_context_define_constant(struct kefir_mem *,
                                             struct kefir_ast_context *,
                                             const char *,
                                             struct kefir_ast_constant_expression *);
kefir_result_t kefir_ast_context_define_tag(struct kefir_mem *,
                                        struct kefir_ast_context *,
                                        const struct kefir_ast_type *);
kefir_result_t kefir_ast_context_define_type(struct kefir_mem *,
                                         struct kefir_ast_context *,
                                         const char *,
                                         const struct kefir_ast_type *);

#endif