#ifndef KEFIR_AST_CONSTANTS_H_
#define KEFIR_AST_CONSTANTS_H_

#include "kefir/core/basic-types.h"

typedef enum kefir_ast_node_type {
    KEFIR_AST_CONSTANT,
    KEFIR_AST_UNARY_OPERATION,
    KEFIR_AST_BINARY_OPERATION
} kefir_ast_node_type_t;

typedef enum kefir_ast_constant_type {
    KEFIR_AST_BOOL_CONSTANT,
    KEFIR_AST_CHAR_CONSTANT,
    KEFIR_AST_INT_CONSTANT,
    KEFIR_AST_UINT_CONSTANT,
    KEFIR_AST_LONG_CONSTANT,
    KEFIR_AST_ULONG_CONSTANT,
    KEFIR_AST_LONG_LONG_CONSTANT,
    KEFIR_AST_ULONG_LONG_CONSTANT,
    KEFIR_AST_FLOAT_CONSTANT,
    KEFIR_AST_DOUBLE_CONSTANT
} kefir_ast_constant_type_t;

typedef enum kefir_ast_unary_operation_type {
    KEFIR_AST_OPERATION_PLUS,
    KEFIR_AST_OPERATION_NEGATE,
    KEFIR_AST_OPERATION_INVERT,
    KEFIR_AST_OPERATION_LOGICAL_NEGATE,
} kefir_ast_unary_operation_type_t;

typedef enum kefir_ast_binary_operation_type {
    KEFIR_AST_OPERATION_ADD,
    KEFIR_AST_OPERATION_SUBTRACT,
    KEFIR_AST_OPERATION_MULTIPLY,
    KEFIR_AST_OPERATION_DIVIDE,
    KEFIR_AST_OPERATION_MODULO,
    KEFIR_AST_OPERATION_SHIFT_LEFT,
    KEFIR_AST_OPERATION_SHIFT_RIGHT
} kefir_ast_binary_operation_type_t;

#endif