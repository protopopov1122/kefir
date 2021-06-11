#ifndef KEFIR_AST_CONTEXT_H_
#define KEFIR_AST_CONTEXT_H_

#include "kefir/core/mem.h"
#include "kefir/ast/scope.h"
#include "kefir/ast/target_environment.h"
#include "kefir/ast/temporaries.h"

typedef struct kefir_ast_context {
    kefir_result_t (*resolve_ordinary_identifier)(const struct kefir_ast_context *, const char *,
                                                  const struct kefir_ast_scoped_identifier **);
    kefir_result_t (*resolve_tag_identifier)(const struct kefir_ast_context *, const char *,
                                             const struct kefir_ast_scoped_identifier **);
    kefir_result_t (*resolve_label_identifier)(const struct kefir_ast_context *, const char *,
                                               const struct kefir_ast_scoped_identifier **);
    kefir_result_t (*allocate_temporary_value)(struct kefir_mem *, const struct kefir_ast_context *,
                                               const struct kefir_ast_type *, struct kefir_ast_temporary_identifier *);

    kefir_result_t (*define_tag)(struct kefir_mem *, const struct kefir_ast_context *, const struct kefir_ast_type *);
    kefir_result_t (*define_constant)(struct kefir_mem *, const struct kefir_ast_context *, const char *,
                                      struct kefir_ast_constant_expression *, const struct kefir_ast_type *);
    kefir_result_t (*define_identifier)(struct kefir_mem *, const struct kefir_ast_context *, kefir_bool_t,
                                        const char *, const struct kefir_ast_type *,
                                        kefir_ast_scoped_identifier_storage_t, kefir_ast_function_specifier_t,
                                        struct kefir_ast_alignment *, struct kefir_ast_initializer *,
                                        const struct kefir_ast_scoped_identifier **);
    kefir_result_t (*reference_label)(struct kefir_mem *, const struct kefir_ast_context *, const char *, kefir_bool_t,
                                      const struct kefir_ast_scoped_identifier **);

    struct kefir_symbol_table *symbols;
    const struct kefir_ast_type_traits *type_traits;
    struct kefir_ast_type_bundle *type_bundle;
    const struct kefir_ast_target_environment *target_env;
    struct kefir_ast_context_temporaries *temporaries;
    kefir_ast_type_analysis_context_t type_analysis_context;

    void *payload;
} kefir_ast_context_t;

#endif
