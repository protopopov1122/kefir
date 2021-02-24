#ifndef KEFIR_AST_CONST_EXPR_H_
#define KEFIR_AST_CONST_EXPR_H_

#include "kefir/core/mem.h"
#include "kefir/ast/base.h"

typedef kefir_int64_t kefir_ast_constant_expression_value_t;

typedef struct kefir_ast_constant_expression {
    kefir_ast_constant_expression_value_t value;
    const struct kefir_ast_node_base *expression;
} kefir_ast_constant_expression_t;

struct kefir_ast_constant_expression *kefir_ast_constant_expression_empty(struct kefir_mem *,
                                                                      kefir_ast_constant_expression_value_t);

kefir_result_t kefir_ast_constant_expression_free(struct kefir_mem *,
                                              struct kefir_ast_constant_expression *);

#endif