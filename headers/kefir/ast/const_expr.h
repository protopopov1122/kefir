#ifndef KEFIR_AST_CONST_EXPR_H_
#define KEFIR_AST_CONST_EXPR_H_

#include "kefir/core/mem.h"
#include "kefir/ast/base.h"

typedef enum kefir_ast_constant_expression_type {
    KEFIR_AST_CONSTANT_EXPRESSION_INTEGER,
    KEFIR_AST_CONSTANT_EXPRESSION_NULL,
    KEFIR_AST_CONSTANT_EXPRESSION_POINTER,
    KEFIR_AST_CONSTANT_EXPRESSION_OFFSET
} kefir_ast_constant_expression_type_t;

typedef kefir_int64_t kefir_ast_constant_expression_value_t;

typedef struct kefir_ast_constant_expression {
    kefir_ast_constant_expression_type_t type;
    kefir_ast_constant_expression_value_t value;
    const struct kefir_ast_node_base *expression;
} kefir_ast_constant_expression_t;

struct kefir_ast_constant_expression *kefir_ast_constant_expression_integer(struct kefir_mem *,
                                                                        kefir_ast_constant_expression_value_t);

struct kefir_ast_constant_expression *kefir_ast_constant_expression_null(struct kefir_mem *);

kefir_result_t kefir_ast_constant_expression_free(struct kefir_mem *,
                                              struct kefir_ast_constant_expression *);

#endif