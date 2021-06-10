#ifndef KEFIR_AST_CONTEXT_MANAGER_H_
#define KEFIR_AST_CONTEXT_MANAGER_H_

#include "kefir/ast/context.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast/local_context.h"

typedef struct kefir_ast_context_manager {
    struct kefir_ast_global_context *global;
    struct kefir_ast_local_context *local;
    struct kefir_ast_context *current;
} kefir_ast_context_manager_t;

kefir_result_t kefir_ast_context_manager_init(struct kefir_ast_global_context *, struct kefir_ast_context_manager *);

kefir_result_t kefir_ast_context_manager_attach_local(struct kefir_ast_local_context *,
                                                      struct kefir_ast_context_manager *);

kefir_result_t kefir_ast_context_manager_detach_local(struct kefir_ast_context_manager *);

#endif
