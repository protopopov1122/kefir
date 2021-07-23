/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
NODE_TRANSLATOR(labeled_statement, struct kefir_ast_labeled_statement);
NODE_TRANSLATOR(case_statement, struct kefir_ast_case_statement);
NODE_TRANSLATOR(goto_statement, struct kefir_ast_goto_statement);
NODE_TRANSLATOR(continue_statement, struct kefir_ast_continue_statement);
NODE_TRANSLATOR(break_statement, struct kefir_ast_break_statement);
NODE_TRANSLATOR(return_statement, struct kefir_ast_return_statement);
#undef NODE_TRANSLATOR

#endif
