#ifndef KEFIR_AST_CONSTANT_EXPRESSION_H_
#define KEFIR_AST_CONSTANT_EXPRESSION_H_

#include "kefir/ast/node.h"
#include "kefir/ast/context.h"
#include "kefir/ast/base.h"

typedef enum kefir_ast_constant_expression_class {
    KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
    KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT,
    KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS
} kefir_ast_constant_expression_class_t;

typedef enum kefir_ast_constant_expression_pointer_base_type {
    KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER,
    KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER,
    KEFIR_AST_CONSTANT_EXPRESSION_POINTER_LITERAL
} kefir_ast_constant_expression_pointer_base_type_t;

typedef kefir_int64_t kefir_ast_constant_expression_int_t;
typedef kefir_float64_t kefir_ast_constant_expression_float_t;

typedef struct kefir_ast_constant_expression_pointer {
    const struct kefir_ast_node_base *pointer_node;

    kefir_ast_constant_expression_pointer_base_type_t type;
    union {
        const char *literal;
        kefir_size_t integral;
    } base;
    kefir_size_t offset;
} kefir_ast_constant_expression_pointer_t;

typedef struct kefir_ast_constant_expression_value {
    kefir_ast_constant_expression_class_t klass;

    kefir_ast_constant_expression_int_t integer;
    kefir_ast_constant_expression_float_t floating_point;
    struct kefir_ast_constant_expression_pointer pointer;
} kefir_ast_constant_expression_value_t;

typedef struct kefir_ast_constant_expression {
    struct kefir_ast_constant_expression_value value;
    const struct kefir_ast_node_base *expression;
} kefir_ast_constant_expression_t;

struct kefir_ast_constant_expression *kefir_ast_new_constant_expression(struct kefir_mem *,
                                                                    const struct kefir_ast_context *,
                                                                    struct kefir_ast_node_base *);

kefir_result_t kefir_ast_constant_expression_free(struct kefir_mem *,
                                              struct kefir_ast_constant_expression *);

kefir_result_t kefir_ast_constant_expression_evaluate(struct kefir_mem *,
                                                  const struct kefir_ast_context *,
                                                  struct kefir_ast_node_base *,
                                                  struct kefir_ast_constant_expression_value *);

struct kefir_ast_constant_expression *kefir_ast_constant_expression_integer(struct kefir_mem *,
                                                                        kefir_ast_constant_expression_int_t);
                                                                        
#endif