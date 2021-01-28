#ifndef KEFIR_AST_NODE_H_
#define KEFIR_AST_NODE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef enum kefir_ast_node_type {
    KEFIR_AST_CONSTANT,
    KEFIR_AST_UNARY_OPERATION,
    KEFIR_AST_BINARY_OPERATION
} kefir_ast_node_type_t;

typedef enum kefir_ast_constant_type {
    KEFIR_AST_INT64_CONSTANT,
    KEFIR_AST_FLOAT32_CONSTANT,
    KEFIR_AST_FLOAT64_CONSTANT
} kefir_ast_constant_type_t;

typedef enum kefir_ast_unary_operation_type {
    KEFIR_AST_OPERATION_NEGATE,
    KEFIR_AST_OPERATION_INVERT
} kefir_ast_unary_operation_type_t;

typedef enum kefir_ast_binary_operation_type {
    KEFIR_AST_OPERATION_ADD,
    KEFIR_AST_OPERATION_SUBTRACT,
    KEFIR_AST_OPERATION_MULTIPLY,
    KEFIR_AST_OPERATION_DIVIDE,
    KEFIR_AST_OPERATION_MODULO,
} kefir_ast_binary_operation_type_t;

typedef struct kefir_ast_node_base kefir_ast_node_base_t;

typedef struct kefir_ast_node_class {
    kefir_ast_node_type_t type;
    kefir_result_t (*free)(struct kefir_mem *, struct kefir_ast_node_base *);
} kefir_ast_node_class_t;

typedef struct kefir_ast_node_base {
    const struct kefir_ast_node_class *klass;
    void *self;
} kefir_ast_node_base_t;

typedef struct kefir_ast_constant {
    struct kefir_ast_node_base base;

    kefir_ast_constant_type_t type;
    union {
        kefir_int64_t i64;
        kefir_float64_t f64;
        kefir_float32_t f32;
    } value;
} kefir_ast_constant_t;

typedef struct kefir_ast_unary_operation {
    struct kefir_ast_node_base base;

    kefir_ast_unary_operation_type_t type;
    struct kefir_ast_node_base *arg;
} kefir_ast_unary_operation_t;

typedef struct kefir_ast_binary_operation {
    struct kefir_ast_node_base base;

    kefir_ast_binary_operation_type_t type;
    struct kefir_ast_node_base *arg1;
    struct kefir_ast_node_base *arg2;
} kefir_ast_binary_operation_t;

struct kefir_ast_node_base *kefir_ast_new_constant_i64(struct kefir_mem *, kefir_int64_t);
struct kefir_ast_node_base *kefir_ast_new_constant_f32(struct kefir_mem *, kefir_float32_t);
struct kefir_ast_node_base *kefir_ast_new_constant_f64(struct kefir_mem *, kefir_float64_t);
struct kefir_ast_node_base *kefir_ast_new_unary_operation(struct kefir_mem *,
                                                      kefir_ast_unary_operation_type_t,
                                                      struct kefir_ast_node_base *);
struct kefir_ast_node_base *kefir_ast_new_binary_operation(struct kefir_mem *,
                                                       kefir_ast_binary_operation_type_t,
                                                       struct kefir_ast_node_base *,
                                                       struct kefir_ast_node_base *);

#define KEFIR_AST_NODE_FREE(mem, base) ((base)->klass->free((mem), (base)))

typedef struct kefir_ast_visitor {
    kefir_result_t (*generic_handler)(const struct kefir_ast_visitor *, const struct kefir_ast_node_base *, void *);
    kefir_result_t (*constant)(const struct kefir_ast_visitor *, const struct kefir_ast_constant *, void *);
    kefir_result_t (*unary_operation)(const struct kefir_ast_visitor *, const struct kefir_ast_unary_operation *, void *);
    kefir_result_t (*binary_operation)(const struct kefir_ast_visitor *, const struct kefir_ast_binary_operation *, void *);
} kefir_ast_visitor_t;

kefir_result_t kefir_ast_visitor_init(struct kefir_ast_visitor *,
                                  kefir_result_t (*)(const struct kefir_ast_visitor *, const struct kefir_ast_node_base *, void *));
kefir_result_t kefir_ast_visitor_visit(const struct kefir_ast_visitor *,
                                   const struct kefir_ast_node_base *,
                                   void *);

#endif