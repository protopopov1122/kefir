#ifndef KEFIR_AST_NODE_H_
#define KEFIR_AST_NODE_H_

#include "kefir/ast/node_base.h"

KEFIR_AST_NODE_STRUCT(kefir_ast_constant, {
    kefir_ast_constant_type_t type;
    union {
        kefir_bool_t boolean;
        kefir_char_t character;
        kefir_int_t integer;
        kefir_uint_t uinteger;
        kefir_long_t long_integer;
        kefir_ulong_t ulong_integer;
        kefir_long_long_t long_long;
        kefir_ulong_long_t ulong_long;
        kefir_float32_t float32;
        kefir_float64_t float64;
    } value;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_unary_operation, {
    kefir_ast_unary_operation_type_t type;
    struct kefir_ast_node_base *arg;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_binary_operation, {
    kefir_ast_binary_operation_type_t type;
    struct kefir_ast_node_base *arg1;
    struct kefir_ast_node_base *arg2;
});


struct kefir_ast_constant *kefir_ast_new_constant_bool(struct kefir_mem *, kefir_bool_t);
struct kefir_ast_constant *kefir_ast_new_constant_char(struct kefir_mem *, kefir_char_t);
struct kefir_ast_constant *kefir_ast_new_constant_int(struct kefir_mem *, kefir_int_t);
struct kefir_ast_constant *kefir_ast_new_constant_uint(struct kefir_mem *, kefir_uint_t);
struct kefir_ast_constant *kefir_ast_new_constant_long(struct kefir_mem *, kefir_long_t);
struct kefir_ast_constant *kefir_ast_new_constant_ulong(struct kefir_mem *, kefir_ulong_t);
struct kefir_ast_constant *kefir_ast_new_constant_long_long(struct kefir_mem *, kefir_long_long_t);
struct kefir_ast_constant *kefir_ast_new_constant_ulong_long(struct kefir_mem *, kefir_ulong_long_t);
struct kefir_ast_constant *kefir_ast_new_constant_float(struct kefir_mem *, kefir_float32_t);
struct kefir_ast_constant *kefir_ast_new_constant_double(struct kefir_mem *, kefir_float64_t);
struct kefir_ast_unary_operation *kefir_ast_new_unary_operation(struct kefir_mem *,
                                                            kefir_ast_unary_operation_type_t,
                                                            struct kefir_ast_node_base *);
struct kefir_ast_binary_operation *kefir_ast_new_binary_operation(struct kefir_mem *,
                                                              kefir_ast_binary_operation_type_t,
                                                              struct kefir_ast_node_base *,
                                                              struct kefir_ast_node_base *);

typedef struct kefir_ast_visitor {
    KEFIR_AST_VISITOR_METHOD(generic_handler, kefir_ast_node_base);
    KEFIR_AST_VISITOR_METHOD(constant, kefir_ast_constant);
    KEFIR_AST_VISITOR_METHOD(unary_operation, kefir_ast_unary_operation);
    KEFIR_AST_VISITOR_METHOD(binary_operation, kefir_ast_binary_operation);
} kefir_ast_visitor_t;

#endif