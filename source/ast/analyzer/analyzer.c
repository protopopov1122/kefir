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
#include "kefir/core/source_error.h"
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/constant_expression.h"
#include "kefir/core/extensions.h"

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
        ASSIGN_DECL_CAST(struct kefir_ast_analysis_parameters *, param, payload);                                  \
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
VISITOR(builtin, struct kefir_ast_builtin)

#undef VISITOR

static kefir_result_t visit_extension_node(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_extension_node *node, void *payload) {
    UNUSED(visitor);
    UNUSED(node);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST analyzer payload"));
    ASSIGN_DECL_CAST(struct kefir_ast_analysis_parameters *, param, payload);
    if (param->context->extensions != NULL && param->context->extensions->analyze_extension_node != NULL) {
        REQUIRE_OK(param->context->extensions->analyze_extension_node(param->mem, param->context, param->base));
    } else {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                      "Extension node analysis procedure is not defined");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                      struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translaction_context"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node base"));
    struct kefir_ast_analysis_parameters param = {.mem = mem, .context = context, .base = base};
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
    visitor.builtin = visit_builtin;
    visitor.extension_node = visit_extension_node;

    kefir_result_t res;
    KEFIR_RUN_EXTENSION(&res, mem, context, before_node_analysis, base, &visitor);
    REQUIRE_OK(res);
    REQUIRE_OK(KEFIR_AST_NODE_VISIT(&visitor, base, &param));
    KEFIR_RUN_EXTENSION(&res, mem, context, after_node_analysis, base);
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_is_null_pointer_constant(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                  const struct kefir_ast_node_base *node, kefir_bool_t *is_null) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node"));
    REQUIRE(is_null != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to boolean"));

    *is_null = false;
    REQUIRE(node->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION &&
                node->properties.expression_props.constant_expression,
            KEFIR_OK);
    REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(node->properties.type) ||
                (node->properties.type->tag == KEFIR_AST_TYPE_SCALAR_POINTER &&
                 node->properties.type->referenced_type->tag == KEFIR_AST_TYPE_VOID),
            KEFIR_OK);

    struct kefir_ast_constant_expression_value value;
    kefir_result_t res = kefir_ast_constant_expression_value_evaluate(mem, context, node, &value);
    if (res == KEFIR_NOT_CONSTANT) {
        return KEFIR_OK;
    } else {
        REQUIRE_OK(res);
    }
    switch (value.klass) {
        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
            if (value.integer == 0) {
                *is_null = true;
            }
            break;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
            if (value.pointer.type == KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER &&
                value.pointer.base.integral == 0 && value.pointer.offset == 0) {
                *is_null = true;
            }
            break;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
            // Intentionally left blank
            break;
    }
    return KEFIR_OK;
}
