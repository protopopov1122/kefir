#ifndef KEFIR_AST_TRANSLATOR_TRANSLATOR_IMPL_H_
#define KEFIR_AST_TRANSLATOR_TRANSLATOR_IMPL_H_

#include "kefir/ast-translator/context.h"
#include "kefir/ast/node.h"
#include "kefir/ir/builder.h"

#define NODE_TRANSLATOR(_id, _type)                                                                            \
    kefir_result_t kefir_ast_translate_##_id##_node(struct kefir_mem *, struct kefir_ast_translator_context *, \
                                                    struct kefir_irbuilder_block *, const _type *)

NODE_TRANSLATOR(constant, struct kefir_ast_constant);
NODE_TRANSLATOR(identifier, struct kefir_ast_identifier);
NODE_TRANSLATOR(generic_selection, struct kefir_ast_generic_selection);
NODE_TRANSLATOR(string_literal, struct kefir_ast_string_literal);
NODE_TRANSLATOR(compound_literal, struct kefir_ast_compound_literal);
NODE_TRANSLATOR(array_subscript, struct kefir_ast_array_subscript);
NODE_TRANSLATOR(struct_member, struct kefir_ast_struct_member);
NODE_TRANSLATOR(function_call, struct kefir_ast_function_call);
NODE_TRANSLATOR(cast_operator, struct kefir_ast_cast_operator);
NODE_TRANSLATOR(unary_operation, struct kefir_ast_unary_operation);
NODE_TRANSLATOR(binary_operation, struct kefir_ast_binary_operation);
NODE_TRANSLATOR(comma_operator, struct kefir_ast_comma_operator);
NODE_TRANSLATOR(conditional_operator, struct kefir_ast_conditional_operator);
NODE_TRANSLATOR(assignment_operator, struct kefir_ast_assignment_operator);
NODE_TRANSLATOR(expression_statement, struct kefir_ast_expression_statement);
NODE_TRANSLATOR(compound_statement, struct kefir_ast_compound_statement);
NODE_TRANSLATOR(conditional_statement, struct kefir_ast_conditional_statement);
NODE_TRANSLATOR(switch_statement, struct kefir_ast_switch_statement);
NODE_TRANSLATOR(while_statement, struct kefir_ast_while_statement);
NODE_TRANSLATOR(do_while_statement, struct kefir_ast_do_while_statement);
NODE_TRANSLATOR(for_statement, struct kefir_ast_for_statement);
NODE_TRANSLATOR(case_statement, struct kefir_ast_case_statement);
NODE_TRANSLATOR(continue_statement, struct kefir_ast_continue_statement);
NODE_TRANSLATOR(break_statement, struct kefir_ast_break_statement);
#undef NODE_TRANSLATOR

#endif
