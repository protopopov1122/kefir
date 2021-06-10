#ifndef CONSTANT_EXPRESSION_H_
#define CONSTANT_EXPRESSION_H_

#include <string.h>

#define ASSERT_INTEGER_CONST_EXPR(_mem, _context, _node, _value)                                   \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER);                        \
        ASSERT(value.integer == (_value));                                                         \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_FLOAT_CONST_EXPR(_mem, _context, _node, _value)                                     \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT);                          \
        ASSERT(DOUBLE_EQUALS(value.floating_point, (_value), DOUBLE_EPSILON));                     \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_IDENTIFIER_CONST_EXPR(_mem, _context, _node, _value, _offset)                       \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS);                        \
        ASSERT(value.pointer.type == KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER);             \
        ASSERT(strcmp(value.pointer.base.literal, (_value)) == 0);                                 \
        ASSERT(value.pointer.offset == (_offset));                                                 \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_LITERAL_CONST_EXPR(_mem, _context, _node, _value)                                   \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS);                        \
        ASSERT(value.pointer.type == KEFIR_AST_CONSTANT_EXPRESSION_POINTER_LITERAL);               \
        ASSERT(strcmp(value.pointer.base.literal, (_value)) == 0);                                 \
        ASSERT(value.pointer.offset == 0);                                                         \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_INTPTR_CONST_EXPR(_mem, _context, _node, _value, _offset)                           \
    do {                                                                                           \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                           \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                               \
        struct kefir_ast_constant_expression_value value;                                          \
        ASSERT_OK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS);                        \
        ASSERT(value.pointer.type == KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER);               \
        ASSERT(value.pointer.base.integral == (_value));                                           \
        ASSERT(value.pointer.offset == (_offset));                                                 \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                              \
    } while (0)

#define ASSERT_CONST_EXPR_NOK(_mem, _context, _node)                                                \
    do {                                                                                            \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node));                            \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base));                                \
        struct kefir_ast_constant_expression_value value;                                           \
        ASSERT_NOK(kefir_ast_constant_expression_value_evaluate((_mem), (_context), base, &value)); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base));                                               \
    } while (0)

#endif
