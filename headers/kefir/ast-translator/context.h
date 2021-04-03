#ifndef KEFIR_AST_TRANSLATOR_CONTEXT_H_
#define KEFIR_AST_TRANSLATOR_CONTEXT_H_

#include "kefir/ast/global_context.h"
#include "kefir/ast/local_context.h"

typedef struct kefir_ast_translator_global_context {
    struct kefir_ast_global_context *ast_context;
} kefir_ast_translator_global_context_t;

#endif