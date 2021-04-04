#ifndef KEFIR_AST_TRANSLATOR_CONTEXT_H_
#define KEFIR_AST_TRANSLATOR_CONTEXT_H_

#include "kefir/ast/context.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ir/module.h"

#define KEFIR_AST_TRANSLATOR_STATIC_VARIABLES_IDENTIFIER "__kefirrt_module_static_vars"

typedef struct kefir_ast_translator_context {
    const struct kefir_ast_context *ast_context;
    const struct kefir_ast_translator_environment *environment;
    struct kefir_ir_module *module;
} kefir_ast_translator_context_t;

kefir_result_t kefir_ast_translator_context_init(struct kefir_ast_translator_context *,
                                             const struct kefir_ast_context *,
                                             const struct kefir_ast_translator_environment *,
                                             struct kefir_ir_module *);

kefir_result_t kefir_ast_translator_context_free(struct kefir_ast_translator_context *);

#endif