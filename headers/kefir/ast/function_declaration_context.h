#ifndef KEFIR_AST_FUNCTION_DECLARATION_CONTEXT_H_
#define KEFIR_AST_FUNCTION_DECLARATION_CONTEXT_H_

#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"
#include "kefir/core/list.h"
#include "kefir/ast/scope.h"
#include "kefir/ast/context.h"

typedef struct kefir_ast_function_declaration_context {
    struct kefir_ast_context context;

    const struct kefir_ast_context *parent;
    struct kefir_ast_identifier_flat_scope ordinary_scope;
    struct kefir_ast_identifier_flat_scope tag_scope;
} kefir_ast_function_declaration_context_t;

kefir_result_t kefir_ast_function_declaration_context_init(struct kefir_mem *, const struct kefir_ast_context *,
                                                           struct kefir_ast_function_declaration_context *);
kefir_result_t kefir_ast_function_declaration_context_free(struct kefir_mem *,
                                                           struct kefir_ast_function_declaration_context *);

#endif
