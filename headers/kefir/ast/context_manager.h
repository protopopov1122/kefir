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

kefir_result_t kefir_ast_context_manager_init(struct kefir_mem *,
                                          const struct kefir_ast_type_traits *,
                                          struct kefir_ast_target_environment *,
                                          struct kefir_ast_context_manager *);

kefir_result_t kefir_ast_context_manager_free(struct kefir_mem *,
                                          struct kefir_ast_context_manager *);

kefir_result_t kefir_ast_context_manager_open_local(struct kefir_mem *,
                                                struct kefir_ast_context_manager *);

kefir_result_t kefir_ast_context_manager_close_local(struct kefir_mem *,
                                                 struct kefir_ast_context_manager *);

#endif