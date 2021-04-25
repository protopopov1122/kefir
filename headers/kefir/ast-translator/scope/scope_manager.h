#ifndef KEFIR_AST_TRANSLATOR_SCOPE_SCOPE_MANAGER_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_SCOPE_MANAGER_H_

#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast/global_context.h"

typedef struct kefir_ast_translator_scope_manager {
    struct kefir_ast_translator_global_scope_layout *global_layout;
    struct kefir_ast_translator_local_scope_layout *local_layout;
} kefir_ast_translator_scope_manager_t;

kefir_result_t kefir_ast_translator_scope_manager_init(struct kefir_mem *,
                                                   struct kefir_ast_translator_context *,
                                                   struct kefir_ast_global_context *,
                                                   struct kefir_ast_translator_scope_manager *);

kefir_result_t kefir_ast_translator_scope_manager_free(struct kefir_mem *,
                                                   struct kefir_ast_translator_context *,
                                                   struct kefir_ast_translator_scope_manager *);

kefir_result_t kefir_ast_translator_scope_manager_open_local(struct kefir_mem *,
                                                         struct kefir_ast_translator_context *,
                                                         struct kefir_ast_local_context *,
                                                         struct kefir_ast_translator_scope_manager *);

kefir_result_t kefir_ast_translator_scope_manager_close_local(struct kefir_mem *,
                                                          struct kefir_ast_translator_context *,
                                                          struct kefir_ast_translator_scope_manager *);

kefir_result_t kefir_ast_translator_scope_manager_define_globals(struct kefir_mem *,
                                                             struct kefir_ast_translator_context *,
                                                             struct kefir_ast_translator_scope_manager *);

#endif