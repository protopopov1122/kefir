#ifndef KEFIR_AST_EXPR_TYPE_H_
#define KEFIR_AST_EXPR_TYPE_H_

#include "kefir/core/mem.h"
#include "kefir/ast/node.h"
#include "kefir/ast/type.h"

kefir_result_t kefir_ast_assign_expression_type(struct kefir_mem *,
                                            struct kefir_ast_type_repository *,
                                            const struct kefir_ast_basic_types *,
                                            struct kefir_ast_node_base *);

#define KEFIR_AST_ASSIGN_EXPRESSION_TYPE(mem, repo, basic_types, base) \
    ((base)->expression_type == NULL ? kefir_ast_assign_expression_type((mem), (repo), (basic_types), (base)) : KEFIR_OK)

#endif