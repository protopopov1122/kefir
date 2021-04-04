#ifndef KEFIR_AST_TRANSLATOR_CONTEXT_H_
#define KEFIR_AST_TRANSLATOR_CONTEXT_H_

#include "kefir/ast/context.h"
#include "kefir/ast-translator/environment.h"

typedef struct kefir_ast_translator_context {
    const struct kefir_ast_context *ast_context;
    const struct kefir_ast_translator_environment *environment;
} kefir_ast_translator_context_t;

kefir_result_t kefir_ast_translator_context_init(struct kefir_ast_translator_context *,
                                             const struct kefir_ast_context *,
                                             const struct kefir_ast_translator_environment *);

kefir_result_t kefir_ast_translator_context_free(struct kefir_ast_translator_context *);

#endif