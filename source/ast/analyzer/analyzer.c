/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"
#include "kefir/ast/analyzer/nodes.h"

struct assign_param {
    struct kefir_mem *mem;
    struct kefir_ast_node_base *base;
    const struct kefir_ast_context *context;
};

static kefir_result_t visit_non_expression(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_node_base *base, void *payload) {
    UNUSED(visitor);
    UNUSED(base);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unimplemented AST node analyzer");
}

#define VISITOR(id, type)                                                                                          \
    static kefir_result_t visit_##id(const struct kefir_ast_visitor *visitor, const type *node, void *payload) {   \
        UNUSED(visitor);                                                                                           \
        REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST analyzer payload")); \
        ASSIGN_DECL_CAST(struct assign_param *, param, payload);                                                   \
        return kefir_ast_analyze_##id##_node(param->mem, param->context, node, param->base);                       \
    }

VISITOR(constant, struct kefir_ast_constant)
VISITOR(identifier, struct kefir_ast_identifier)
VISITOR(unary_operation, struct kefir_ast_unary_operation)
VISITOR(binary_operation, struct kefir_ast_binary_operation)
VISITOR(array_subscript, struct kefir_ast_array_subscript)
VISITOR(function_call, struct kefir_ast_function_call)
VISITOR(struct_member, struct kefir_ast_struct_member)
VISITOR(string_literal, struct kefir_ast_string_literal)
VISITOR(type_name, struct kefir_ast_type_name)
VISITOR(init_declarator, struct kefir_ast_init_declarator)
VISITOR(declaration, struct kefir_ast_declaration)
VISITOR(cast_operator, struct kefir_ast_cast_operator)
VISITOR(generic_selection, struct kefir_ast_generic_selection)
VISITOR(conditional_operator, struct kefir_ast_conditional_operator)
VISITOR(assignment_operator, struct kefir_ast_assignment_operator)
VISITOR(comma_operator, struct kefir_ast_comma_operator)
VISITOR(compound_literal, struct kefir_ast_compound_literal)
VISITOR(static_assertion, struct kefir_ast_static_assertion)
VISITOR(labeled_statement, struct kefir_ast_labeled_statement)
VISITOR(case_statement, struct kefir_ast_case_statement)
VISITOR(expression_statement, struct kefir_ast_expression_statement)
VISITOR(compound_statement, struct kefir_ast_compound_statement)
VISITOR(conditional_statement, struct kefir_ast_conditional_statement)
VISITOR(switch_statement, struct kefir_ast_switch_statement)
VISITOR(while_statement, struct kefir_ast_while_statement)
VISITOR(do_while_statement, struct kefir_ast_do_while_statement)
VISITOR(for_statement, struct kefir_ast_for_statement)
VISITOR(goto_statement, struct kefir_ast_goto_statement)
VISITOR(continue_statement, struct kefir_ast_continue_statement)
VISITOR(break_statement, struct kefir_ast_break_statement)
VISITOR(return_statement, struct kefir_ast_return_statement)
VISITOR(function_definition, struct kefir_ast_function_definition)
VISITOR(translation_unit, struct kefir_ast_translation_unit)

#undef VISITOR

kefir_result_t kefir_ast_analyze_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                      struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translaction_context"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node base"));
    struct assign_param param = {.mem = mem, .context = context, .base = base};
    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, visit_non_expression));
    visitor.constant = visit_constant;
    visitor.identifier = visit_identifier;
    visitor.unary_operation = visit_unary_operation;
    visitor.binary_operation = visit_binary_operation;
    visitor.array_subscript = visit_array_subscript;
    visitor.function_call = visit_function_call;
    visitor.struct_member = visit_struct_member;
    visitor.struct_indirect_member = visit_struct_member;
    visitor.string_literal = visit_string_literal;
    visitor.type_name = visit_type_name;
    visitor.init_declarator = visit_init_declarator;
    visitor.declaration = visit_declaration;
    visitor.cast_operator = visit_cast_operator;
    visitor.generic_selection = visit_generic_selection;
    visitor.conditional_operator = visit_conditional_operator;
    visitor.assignment_operator = visit_assignment_operator;
    visitor.comma_operator = visit_comma_operator;
    visitor.compound_literal = visit_compound_literal;
    visitor.static_assertion = visit_static_assertion;
    visitor.labeled_statement = visit_labeled_statement;
    visitor.case_statement = visit_case_statement;
    visitor.expression_statement = visit_expression_statement;
    visitor.compound_statement = visit_compound_statement;
    visitor.conditional_statement = visit_conditional_statement;
    visitor.switch_statement = visit_switch_statement;
    visitor.while_statement = visit_while_statement;
    visitor.do_while_statement = visit_do_while_statement;
    visitor.for_statement = visit_for_statement;
    visitor.goto_statement = visit_goto_statement;
    visitor.continue_statement = visit_continue_statement;
    visitor.break_statement = visit_break_statement;
    visitor.return_statement = visit_return_statement;
    visitor.function_definition = visit_function_definition;
    visitor.translation_unit = visit_translation_unit;
    return KEFIR_AST_NODE_VISIT(&visitor, base, &param);
}
