#ifndef KEFIR_AST_TRANSLATOR_SCOPE_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_H_

#include "kefir/core/mem.h"
#include "kefir/ast/type.h"
#include "kefir/ast/constants.h"
#include "kefir/core/hashtree.h"

typedef struct kefir_ast_identifier_scope_iterator {
    struct kefir_hashtree_node_iterator iter;
} kefir_ast_identifier_scope_iterator_t;

typedef struct kefir_ast_identifier_scope {
    struct kefir_hashtree content;
} kefir_ast_identifier_scope_t;

kefir_result_t kefir_ast_identifier_scope_init(struct kefir_ast_identifier_scope *);
kefir_result_t kefir_ast_identifier_scope_free(struct kefir_mem *,
                                           struct kefir_ast_identifier_scope *);
kefir_result_t kefir_ast_identifier_scope_insert(struct kefir_mem *,
                                             struct kefir_ast_identifier_scope *,
                                             const char *,
                                             const struct kefir_ast_type *);
kefir_result_t kefir_ast_identifier_scope_at(const struct kefir_ast_identifier_scope *,
                                         const char *,
                                         const struct kefir_ast_type **);
kefir_result_t kefir_ast_identifier_scope_iter(const struct kefir_ast_identifier_scope *,
                                           struct kefir_ast_identifier_scope_iterator *,
                                           const char **,
                                           const struct kefir_ast_type **);
kefir_result_t kefir_ast_identifier_scope_next(const struct kefir_ast_identifier_scope *,
                                           struct kefir_ast_identifier_scope_iterator *,
                                           const char **,
                                           const struct kefir_ast_type **);


#endif