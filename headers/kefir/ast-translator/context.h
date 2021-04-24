#ifndef KEFIR_AST_TRANSLATOR_CONTEXT_H_
#define KEFIR_AST_TRANSLATOR_CONTEXT_H_

#include "kefir/ast/context.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ast-translator/type_resolver_stack.h"
#include "kefir/ir/module.h"

typedef struct kefir_ast_translator_context {
    const struct kefir_ast_context *ast_context;
    const struct kefir_ast_translator_environment *environment;
    struct kefir_ir_module *module;
    struct kefir_ast_translator_type_resolver_stack type_resolver;
} kefir_ast_translator_context_t;

kefir_result_t kefir_ast_translator_context_init(struct kefir_mem *,
                                             struct kefir_ast_translator_context *,
                                             const struct kefir_ast_context *,
                                             const struct kefir_ast_translator_environment *,
                                             struct kefir_ir_module *);

kefir_result_t kefir_ast_translator_context_free(struct kefir_mem *,
                                             struct kefir_ast_translator_context *);

struct kefir_ast_translator_type_resolver *kefir_ast_translator_context_type_resolver(struct kefir_ast_translator_context *);

#endif