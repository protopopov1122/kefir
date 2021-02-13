#ifndef KEFIR_AST_TRANSLATOR_CONTEXT_H_
#define KEFIR_AST_TRANSLATOR_CONTEXT_H_

#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"
#include "kefir/ast/translator/scope.h"

typedef struct kefir_ast_global_translation_context {
    struct kefir_symbol_table symbols;
    const struct kefir_ast_type_traits *type_traits;
    struct kefir_ast_type_repository type_repository;
    struct kefir_ast_identifier_flat_scope object_scope;
} kefir_ast_global_translation_context_t;

typedef struct kefir_ast_translation_context {
    struct kefir_ast_global_translation_context *global;
    struct kefir_ast_identifier_block_scope local_object_scope;
} kefir_ast_translation_context_t;

kefir_result_t kefir_ast_translation_global_context_init(struct kefir_mem *,
                                                     const struct kefir_ast_type_traits *,
                                                     struct kefir_ast_global_translation_context *);
kefir_result_t kefir_ast_translation_global_context_free(struct kefir_mem *,
                                                     struct kefir_ast_global_translation_context *);

kefir_result_t kefir_ast_translation_context_init(struct kefir_mem *,
                                              struct kefir_ast_global_translation_context *,
                                              struct kefir_ast_translation_context *);
kefir_result_t kefir_ast_translation_context_free(struct kefir_mem *,
                                              struct kefir_ast_translation_context *);

kefir_result_t kefir_ast_translation_context_resolve_object_identifier(const struct kefir_ast_translation_context *,
                                                            const char *,
                                                            const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_translation_context_declare_local_object_identifier(struct kefir_mem *,
                                                                  struct kefir_ast_translation_context *,
                                                                  const char *,
                                                                  const struct kefir_ast_scoped_identifier *);
kefir_result_t kefir_ast_translation_context_declare_global_object_identifier(struct kefir_mem *,
                                                                   struct kefir_ast_translation_context *,
                                                                   const char *,
                                                                   const struct kefir_ast_scoped_identifier *);
kefir_result_t kefir_ast_translation_context_push_block_scope(struct kefir_mem *,
                                                          struct kefir_ast_translation_context *);
kefir_result_t kefir_ast_translation_context_pop_block_scope(struct kefir_ast_translation_context *);

#endif