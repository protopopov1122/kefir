#ifndef KEFIR_AST_CONTEXT_IMPL_H_
#define KEFIR_AST_CONTEXT_IMPL_H_

#include "kefir/core/mem.h"
#include "kefir/ast/scope.h"

kefir_result_t kefir_ast_context_free_scoped_identifier(struct kefir_mem *, struct kefir_ast_scoped_identifier *,
                                                        void *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_object_identifier(
    struct kefir_mem *, const struct kefir_ast_type *, kefir_ast_scoped_identifier_storage_t,
    struct kefir_ast_alignment *, kefir_ast_scoped_identifier_linkage_t, kefir_bool_t, struct kefir_ast_initializer *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_constant(struct kefir_mem *,
                                                                               struct kefir_ast_constant_expression *,
                                                                               const struct kefir_ast_type *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_type_tag(struct kefir_mem *,
                                                                               const struct kefir_ast_type *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_type_definition(struct kefir_mem *,
                                                                                      const struct kefir_ast_type *);

kefir_result_t kefir_ast_context_type_retrieve_tag(const struct kefir_ast_type *, const char **);

kefir_result_t kefir_ast_context_update_existing_scoped_type_tag(struct kefir_ast_scoped_identifier *,
                                                                 const struct kefir_ast_type *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_function_identifier(
    struct kefir_mem *, const struct kefir_ast_type *, kefir_ast_function_specifier_t,
    kefir_ast_scoped_identifier_storage_t, kefir_bool_t);

kefir_ast_function_specifier_t kefir_ast_context_merge_function_specifiers(kefir_ast_function_specifier_t,
                                                                           kefir_ast_function_specifier_t);

kefir_result_t kefir_ast_context_merge_alignment(struct kefir_mem *, struct kefir_ast_alignment **,
                                                 struct kefir_ast_alignment *);

#endif
