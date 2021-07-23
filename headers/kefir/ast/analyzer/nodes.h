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

#ifndef KEFIR_AST_ANALYZER_NODES_H_
#define KEFIR_AST_ANALYZER_NODES_H_

#include "kefir/ast/node.h"
#include "kefir/ast/context.h"

#define ANALYZE_NODE(id, type)                                                                                       \
    kefir_result_t kefir_ast_analyze_##id##_node(struct kefir_mem *, const struct kefir_ast_context *, const type *, \
                                                 struct kefir_ast_node_base *)

ANALYZE_NODE(constant, struct kefir_ast_constant);
ANALYZE_NODE(identifier, struct kefir_ast_identifier);
ANALYZE_NODE(unary_operation, struct kefir_ast_unary_operation);
ANALYZE_NODE(binary_operation, struct kefir_ast_binary_operation);
ANALYZE_NODE(array_subscript, struct kefir_ast_array_subscript);
ANALYZE_NODE(function_call, struct kefir_ast_function_call);
ANALYZE_NODE(struct_member, struct kefir_ast_struct_member);
ANALYZE_NODE(string_literal, struct kefir_ast_string_literal);
ANALYZE_NODE(declaration, struct kefir_ast_declaration);
ANALYZE_NODE(type_name, struct kefir_ast_type_name);
ANALYZE_NODE(cast_operator, struct kefir_ast_cast_operator);
ANALYZE_NODE(generic_selection, struct kefir_ast_generic_selection);
ANALYZE_NODE(conditional_operator, struct kefir_ast_conditional_operator);
ANALYZE_NODE(assignment_operator, struct kefir_ast_assignment_operator);
ANALYZE_NODE(comma_operator, struct kefir_ast_comma_operator);
ANALYZE_NODE(compound_literal, struct kefir_ast_compound_literal);
ANALYZE_NODE(static_assertion, struct kefir_ast_static_assertion);
ANALYZE_NODE(labeled_statement, struct kefir_ast_labeled_statement);
ANALYZE_NODE(case_statement, struct kefir_ast_case_statement);
ANALYZE_NODE(expression_statement, struct kefir_ast_expression_statement);
ANALYZE_NODE(compound_statement, struct kefir_ast_compound_statement);
ANALYZE_NODE(conditional_statement, struct kefir_ast_conditional_statement);
ANALYZE_NODE(switch_statement, struct kefir_ast_switch_statement);
ANALYZE_NODE(while_statement, struct kefir_ast_while_statement);
ANALYZE_NODE(do_while_statement, struct kefir_ast_do_while_statement);
ANALYZE_NODE(for_statement, struct kefir_ast_for_statement);
ANALYZE_NODE(goto_statement, struct kefir_ast_goto_statement);
ANALYZE_NODE(continue_statement, struct kefir_ast_continue_statement);
ANALYZE_NODE(break_statement, struct kefir_ast_break_statement);
ANALYZE_NODE(return_statement, struct kefir_ast_return_statement);
ANALYZE_NODE(function_definition, struct kefir_ast_function_definition);
ANALYZE_NODE(translation_unit, struct kefir_ast_translation_unit);

#undef ANALYZE_NODE

#endif
