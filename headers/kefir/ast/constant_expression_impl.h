#ifndef KEFIR_AST_CONSTANT_EXPRESSION_IMPL_H_
#define KEFIR_AST_CONSTANT_EXPRESSION_IMPL_H_

#include "kefir/ast/constant_expression.h"
#include "kefir/ast/context.h"

#define EVAL(_id, _type) \
    kefir_result_t kefir_ast_evaluate_##_id##_node(struct kefir_mem *, \
                                               const struct kefir_ast_context *, \
                                               const _type *, \
                                               struct kefir_ast_constant_expression_value *)

EVAL(scalar, struct kefir_ast_constant);
EVAL(identifier, struct kefir_ast_identifier);
EVAL(string_literal, struct kefir_ast_string_literal);
EVAL(generic_selection, struct kefir_ast_generic_selection);
EVAL(unary_operation, struct kefir_ast_unary_operation);
// EVAL(binary_operation, struct kefir_ast_binary_operation);
#undef EVAL

#endif