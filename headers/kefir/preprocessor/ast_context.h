#ifndef KEFIR_PREPROCESSOR_AST_CONTEXT_H_
#define KEFIR_PREPROCESSOR_AST_CONTEXT_H_

#include "kefir/ast/context.h"

typedef struct kefir_preprocessor_ast_context {
    struct kefir_ast_context context;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_context_configuration configuration;
} kefir_preprocessor_ast_context_t;

kefir_result_t kefir_preprocessor_ast_context_init(struct kefir_mem *, struct kefir_preprocessor_ast_context *,
                                                   struct kefir_symbol_table *, const struct kefir_ast_type_traits *,
                                                   const struct kefir_ast_target_environment *,
                                                   const struct kefir_ast_context_extensions *);

kefir_result_t kefir_preprocessor_ast_context_free(struct kefir_mem *, struct kefir_preprocessor_ast_context *);

#endif
