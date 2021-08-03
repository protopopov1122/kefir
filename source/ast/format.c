/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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

#include "kefir/ast/format.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t visit_not_impl(const struct kefir_ast_visitor *visitor, const struct kefir_ast_node_base *node,
                                     void *payload) {
    UNUSED(visitor);
    UNUSED(node);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "AST node JSON formatter is not implemented yet");
}

static kefir_result_t visit_identifier(const struct kefir_ast_visitor *visitor, const struct kefir_ast_identifier *node,
                                       void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "identifier"));
    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
    REQUIRE_OK(kefir_json_output_string(json, node->identifier));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_constant(const struct kefir_ast_visitor *visitor, const struct kefir_ast_constant *node,
                                     void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "constant"));
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    switch (node->type) {
        case KEFIR_AST_BOOL_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "boolean"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_boolean(json, node->value.boolean));
            break;

        case KEFIR_AST_CHAR_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "character"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.character));
            break;

        case KEFIR_AST_INT_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "integer"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_integer(json, node->value.integer));
            break;

        case KEFIR_AST_UINT_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "unsigned_integer"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.uinteger));
            break;

        case KEFIR_AST_LONG_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "long"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_integer(json, node->value.long_integer));
            break;

        case KEFIR_AST_ULONG_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "unsigned_long"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.ulong_integer));
            break;

        case KEFIR_AST_LONG_LONG_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "long_long"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.long_long));
            break;

        case KEFIR_AST_ULONG_LONG_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "unsigned_long_long"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.ulong_long));
            break;

        case KEFIR_AST_FLOAT_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "float"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_float(json, node->value.float32));
            break;

        case KEFIR_AST_DOUBLE_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "double"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_float(json, node->value.float64));
            break;
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_string_literal(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_string_literal *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST string literal node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "string_literal"));
    REQUIRE_OK(kefir_json_output_object_key(json, "content"));
    REQUIRE_OK(kefir_json_output_string(json, node->literal));
    REQUIRE_OK(kefir_json_output_object_key(json, "length"));
    REQUIRE_OK(kefir_json_output_uinteger(json, node->length));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_array_subscript(const struct kefir_ast_visitor *visitor,
                                            const struct kefir_ast_array_subscript *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST array subscript node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "array_subscript"));
    REQUIRE_OK(kefir_json_output_object_key(json, "array"));
    REQUIRE_OK(kefir_ast_format(json, node->array));
    REQUIRE_OK(kefir_json_output_object_key(json, "subscript"));
    REQUIRE_OK(kefir_ast_format(json, node->subscript));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_function_call(const struct kefir_ast_visitor *visitor,
                                          const struct kefir_ast_function_call *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function call node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "function_call"));
    REQUIRE_OK(kefir_json_output_object_key(json, "function"));
    REQUIRE_OK(kefir_ast_format(json, node->function));
    REQUIRE_OK(kefir_json_output_object_key(json, "arguments"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->arguments); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, iter->value);
        REQUIRE_OK(kefir_ast_format(json, arg));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_struct_member(const struct kefir_ast_visitor *visitor,
                                          const struct kefir_ast_struct_member *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST struct member node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    if (node->base.klass->type == KEFIR_AST_STRUCTURE_INDIRECT_MEMBER) {
        REQUIRE_OK(kefir_json_output_string(json, "indirect_struct_member"));
    } else {
        REQUIRE_OK(kefir_json_output_string(json, "struct_member"));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "structure"));
    REQUIRE_OK(kefir_ast_format(json, node->structure));
    REQUIRE_OK(kefir_json_output_object_key(json, "member"));
    REQUIRE_OK(kefir_json_output_string(json, node->member));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_unary_operation(const struct kefir_ast_visitor *visitor,
                                            const struct kefir_ast_unary_operation *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST unary operation node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "unary_operation"));
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    switch (node->type) {
        case KEFIR_AST_OPERATION_PLUS:
            REQUIRE_OK(kefir_json_output_string(json, "plus"));
            break;

        case KEFIR_AST_OPERATION_NEGATE:
            REQUIRE_OK(kefir_json_output_string(json, "negate"));
            break;

        case KEFIR_AST_OPERATION_INVERT:
            REQUIRE_OK(kefir_json_output_string(json, "invert"));
            break;

        case KEFIR_AST_OPERATION_LOGICAL_NEGATE:
            REQUIRE_OK(kefir_json_output_string(json, "logical_negate"));
            break;

        case KEFIR_AST_OPERATION_POSTFIX_INCREMENT:
            REQUIRE_OK(kefir_json_output_string(json, "postfix_increment"));
            break;

        case KEFIR_AST_OPERATION_POSTFIX_DECREMENT:
            REQUIRE_OK(kefir_json_output_string(json, "postfix_decrement"));
            break;

        case KEFIR_AST_OPERATION_PREFIX_INCREMENT:
            REQUIRE_OK(kefir_json_output_string(json, "prefix_increment"));
            break;

        case KEFIR_AST_OPERATION_PREFIX_DECREMENT:
            REQUIRE_OK(kefir_json_output_string(json, "prefix_decrement"));
            break;

        case KEFIR_AST_OPERATION_ADDRESS:
            REQUIRE_OK(kefir_json_output_string(json, "address"));
            break;

        case KEFIR_AST_OPERATION_INDIRECTION:
            REQUIRE_OK(kefir_json_output_string(json, "indirection"));
            break;

        case KEFIR_AST_OPERATION_SIZEOF:
            REQUIRE_OK(kefir_json_output_string(json, "sizeof"));
            break;

        case KEFIR_AST_OPERATION_ALIGNOF:
            REQUIRE_OK(kefir_json_output_string(json, "alignof"));
            break;
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "argument"));
    REQUIRE_OK(kefir_ast_format(json, node->arg));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_binary_operation(const struct kefir_ast_visitor *visitor,
                                             const struct kefir_ast_binary_operation *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST binary operation node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "binary_operation"));
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    switch (node->type) {
        case KEFIR_AST_OPERATION_ADD:
            REQUIRE_OK(kefir_json_output_string(json, "add"));
            break;

        case KEFIR_AST_OPERATION_SUBTRACT:
            REQUIRE_OK(kefir_json_output_string(json, "subtract"));
            break;

        case KEFIR_AST_OPERATION_MULTIPLY:
            REQUIRE_OK(kefir_json_output_string(json, "multiply"));
            break;

        case KEFIR_AST_OPERATION_DIVIDE:
            REQUIRE_OK(kefir_json_output_string(json, "divide"));
            break;

        case KEFIR_AST_OPERATION_MODULO:
            REQUIRE_OK(kefir_json_output_string(json, "modulo"));
            break;

        case KEFIR_AST_OPERATION_SHIFT_LEFT:
            REQUIRE_OK(kefir_json_output_string(json, "shift_left"));
            break;

        case KEFIR_AST_OPERATION_SHIFT_RIGHT:
            REQUIRE_OK(kefir_json_output_string(json, "shift_right"));
            break;

        case KEFIR_AST_OPERATION_LESS:
            REQUIRE_OK(kefir_json_output_string(json, "less"));
            break;

        case KEFIR_AST_OPERATION_LESS_EQUAL:
            REQUIRE_OK(kefir_json_output_string(json, "less_equal"));
            break;

        case KEFIR_AST_OPERATION_GREATER:
            REQUIRE_OK(kefir_json_output_string(json, "greater"));
            break;

        case KEFIR_AST_OPERATION_GREATER_EQUAL:
            REQUIRE_OK(kefir_json_output_string(json, "greater_equal"));
            break;

        case KEFIR_AST_OPERATION_EQUAL:
            REQUIRE_OK(kefir_json_output_string(json, "equal"));
            break;

        case KEFIR_AST_OPERATION_NOT_EQUAL:
            REQUIRE_OK(kefir_json_output_string(json, "not_equal"));
            break;

        case KEFIR_AST_OPERATION_BITWISE_AND:
            REQUIRE_OK(kefir_json_output_string(json, "bitwise_and"));
            break;

        case KEFIR_AST_OPERATION_BITWISE_OR:
            REQUIRE_OK(kefir_json_output_string(json, "bitwise_or"));
            break;

        case KEFIR_AST_OPERATION_BITWISE_XOR:
            REQUIRE_OK(kefir_json_output_string(json, "bitwise_xor"));
            break;

        case KEFIR_AST_OPERATION_LOGICAL_AND:
            REQUIRE_OK(kefir_json_output_string(json, "logical_and"));
            break;

        case KEFIR_AST_OPERATION_LOGICAL_OR:
            REQUIRE_OK(kefir_json_output_string(json, "logical_or"));
            break;
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "left"));
    REQUIRE_OK(kefir_ast_format(json, node->arg1));
    REQUIRE_OK(kefir_json_output_object_key(json, "right"));
    REQUIRE_OK(kefir_ast_format(json, node->arg2));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_conditional_operator(const struct kefir_ast_visitor *visitor,
                                                 const struct kefir_ast_conditional_operator *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST conditional operation node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "conditional_operator"));
    REQUIRE_OK(kefir_json_output_object_key(json, "condition"));
    REQUIRE_OK(kefir_ast_format(json, node->condition));
    REQUIRE_OK(kefir_json_output_object_key(json, "thenBranch"));
    REQUIRE_OK(kefir_ast_format(json, node->expr1));
    REQUIRE_OK(kefir_json_output_object_key(json, "elseBranch"));
    REQUIRE_OK(kefir_ast_format(json, node->expr2));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_assignment_operator(const struct kefir_ast_visitor *visitor,
                                                const struct kefir_ast_assignment_operator *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST assignment operator node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "assignment_operator"));
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    switch (node->operation) {
        case KEFIR_AST_ASSIGNMENT_SIMPLE:
            REQUIRE_OK(kefir_json_output_string(json, "simple"));
            break;

        case KEFIR_AST_ASSIGNMENT_MULTIPLY:
            REQUIRE_OK(kefir_json_output_string(json, "multiply"));
            break;

        case KEFIR_AST_ASSIGNMENT_DIVIDE:
            REQUIRE_OK(kefir_json_output_string(json, "divide"));
            break;

        case KEFIR_AST_ASSIGNMENT_MODULO:
            REQUIRE_OK(kefir_json_output_string(json, "modulo"));
            break;

        case KEFIR_AST_ASSIGNMENT_ADD:
            REQUIRE_OK(kefir_json_output_string(json, "add"));
            break;

        case KEFIR_AST_ASSIGNMENT_SUBTRACT:
            REQUIRE_OK(kefir_json_output_string(json, "subtract"));
            break;

        case KEFIR_AST_ASSIGNMENT_SHIFT_LEFT:
            REQUIRE_OK(kefir_json_output_string(json, "shift_left"));
            break;

        case KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT:
            REQUIRE_OK(kefir_json_output_string(json, "shift_right"));
            break;

        case KEFIR_AST_ASSIGNMENT_BITWISE_AND:
            REQUIRE_OK(kefir_json_output_string(json, "bitwise_and"));
            break;

        case KEFIR_AST_ASSIGNMENT_BITWISE_OR:
            REQUIRE_OK(kefir_json_output_string(json, "bitwise_or"));
            break;

        case KEFIR_AST_ASSIGNMENT_BITWISE_XOR:
            REQUIRE_OK(kefir_json_output_string(json, "bitwise_xor"));
            break;
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "target"));
    REQUIRE_OK(kefir_ast_format(json, node->target));
    REQUIRE_OK(kefir_json_output_object_key(json, "value"));
    REQUIRE_OK(kefir_ast_format(json, node->value));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_comma_operator(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_comma_operator *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST assignment operator node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "comma_operator"));
    REQUIRE_OK(kefir_json_output_object_key(json, "expressions"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->expressions); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, expr, iter->value);
        REQUIRE_OK(kefir_ast_format(json, expr));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_format(struct kefir_json_output *json, const struct kefir_ast_node_base *node) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid JSON output"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node"));

    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, visit_not_impl));
    visitor.identifier = visit_identifier;
    visitor.constant = visit_constant;
    visitor.string_literal = visit_string_literal;
    visitor.array_subscript = visit_array_subscript;
    visitor.function_call = visit_function_call;
    visitor.struct_member = visit_struct_member;
    visitor.struct_indirect_member = visit_struct_member;
    visitor.unary_operation = visit_unary_operation;
    visitor.binary_operation = visit_binary_operation;
    visitor.conditional_operator = visit_conditional_operator;
    visitor.assignment_operator = visit_assignment_operator;
    visitor.comma_operator = visit_comma_operator;
    REQUIRE_OK(node->klass->visit(node, &visitor, json));
    return KEFIR_OK;
}

static kefir_result_t format_struct(struct kefir_json_output *json,
                                    const struct kefir_ast_declarator_specifier *specifier) {
    if (specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_STRUCT) {
        REQUIRE_OK(kefir_json_output_string(json, "struct"));
    } else {
        REQUIRE_OK(kefir_json_output_string(json, "union"));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "members"));
    if (specifier->type_specifier.value.structure->complete) {
        REQUIRE_OK(kefir_json_output_array_begin(json));
        for (const struct kefir_list_entry *iter = kefir_list_head(&specifier->type_specifier.value.structure->entries);
             iter != NULL; kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry *, entry, iter->value);

            REQUIRE_OK(kefir_json_output_object_begin(json));
            REQUIRE_OK(kefir_json_output_object_key(json, "type"));
            if (entry->is_static_assertion) {
                REQUIRE_OK(kefir_json_output_string(json, "static_assert"));
                REQUIRE_OK(kefir_json_output_object_key(json, "assertion"));
                REQUIRE_OK(kefir_ast_format(json, KEFIR_AST_NODE_BASE(entry->static_assertion)));
            } else {
                REQUIRE_OK(kefir_json_output_string(json, "declaration"));
                REQUIRE_OK(kefir_json_output_object_key(json, "specifiers"));
                REQUIRE_OK(kefir_ast_format_declarator_specifier_list(json, &entry->declaration.specifiers));
                REQUIRE_OK(kefir_json_output_object_key(json, "declarators"));
                REQUIRE_OK(kefir_json_output_array_begin(json));
                for (const struct kefir_list_entry *iter2 = kefir_list_head(&entry->declaration.declarators);
                     iter2 != NULL; kefir_list_next(&iter2)) {
                    ASSIGN_DECL_CAST(struct kefir_ast_structure_entry_declarator *, declarator, iter2->value);
                    REQUIRE_OK(kefir_json_output_object_begin(json));
                    REQUIRE_OK(kefir_json_output_object_key(json, "declarator"));
                    REQUIRE_OK(kefir_ast_format_declarator(json, declarator->declarator));
                    REQUIRE_OK(kefir_json_output_object_key(json, "bitwidth"));
                    if (declarator->bitwidth != NULL) {
                        REQUIRE_OK(kefir_ast_format(json, declarator->bitwidth));
                    } else {
                        REQUIRE_OK(kefir_json_output_null(json));
                    }
                    REQUIRE_OK(kefir_json_output_object_end(json));
                }
                REQUIRE_OK(kefir_json_output_array_end(json));
            }
            REQUIRE_OK(kefir_json_output_object_end(json));
        }
        REQUIRE_OK(kefir_json_output_array_end(json));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    return KEFIR_OK;
}

static kefir_result_t format_enum(struct kefir_json_output *json,
                                  const struct kefir_ast_declarator_specifier *specifier) {
    REQUIRE_OK(kefir_json_output_string(json, "enum"));
    REQUIRE_OK(kefir_json_output_object_key(json, "members"));
    if (specifier->type_specifier.value.enumeration->complete) {
        REQUIRE_OK(kefir_json_output_array_begin(json));
        for (const struct kefir_list_entry *iter = kefir_list_head(&specifier->type_specifier.value.structure->entries);
             iter != NULL; kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, entry, iter->value);
            REQUIRE_OK(kefir_json_output_object_begin(json));
            REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
            REQUIRE_OK(kefir_json_output_string(json, entry->constant));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            if (entry->value != NULL) {
                REQUIRE_OK(kefir_ast_format(json, entry->value));
            } else {
                REQUIRE_OK(kefir_json_output_null(json));
            }
            REQUIRE_OK(kefir_json_output_object_end(json));
        }
        REQUIRE_OK(kefir_json_output_array_end(json));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_format_declarator_specifier(struct kefir_json_output *json,
                                                     const struct kefir_ast_declarator_specifier *specifier) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid JSON output"));
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator specifier"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    switch (specifier->klass) {
        case KEFIR_AST_TYPE_SPECIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "type_specifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "specifier"));
            switch (specifier->type_specifier.specifier) {
                case KEFIR_AST_TYPE_SPECIFIER_VOID:
                    REQUIRE_OK(kefir_json_output_string(json, "void"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_CHAR:
                    REQUIRE_OK(kefir_json_output_string(json, "char"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_SHORT:
                    REQUIRE_OK(kefir_json_output_string(json, "short"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_INT:
                    REQUIRE_OK(kefir_json_output_string(json, "int"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_LONG:
                    REQUIRE_OK(kefir_json_output_string(json, "long"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_FLOAT:
                    REQUIRE_OK(kefir_json_output_string(json, "float"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_DOUBLE:
                    REQUIRE_OK(kefir_json_output_string(json, "double"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_SIGNED:
                    REQUIRE_OK(kefir_json_output_string(json, "signed"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_UNSIGNED:
                    REQUIRE_OK(kefir_json_output_string(json, "unsigned"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_BOOL:
                    REQUIRE_OK(kefir_json_output_string(json, "bool"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_COMPLEX:
                    REQUIRE_OK(kefir_json_output_string(json, "complex"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_ATOMIC:
                    REQUIRE_OK(kefir_json_output_string(json, "atomic"));
                    REQUIRE_OK(kefir_json_output_object_key(json, "atomic"));
                    REQUIRE_OK(kefir_ast_format(json, specifier->type_specifier.value.atomic_type));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_STRUCT:
                case KEFIR_AST_TYPE_SPECIFIER_UNION:
                    REQUIRE_OK(format_struct(json, specifier));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_ENUM:
                    REQUIRE_OK(format_enum(json, specifier));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_TYPEDEF:
                    REQUIRE_OK(kefir_json_output_string(json, "typedef"));
                    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
                    REQUIRE_OK(kefir_json_output_string(json, specifier->type_specifier.value.type_name));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected type specifier");
            }
            break;

        case KEFIR_AST_TYPE_QUALIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "type_qualifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "qualifier"));
            switch (specifier->type_qualifier) {
                case KEFIR_AST_TYPE_QUALIFIER_CONST:
                    REQUIRE_OK(kefir_json_output_string(json, "const"));
                    break;

                case KEFIR_AST_TYPE_QUALIFIER_RESTRICT:
                    REQUIRE_OK(kefir_json_output_string(json, "restrict"));
                    break;

                case KEFIR_AST_TYPE_QUALIFIER_VOLATILE:
                    REQUIRE_OK(kefir_json_output_string(json, "volatile"));
                    break;

                case KEFIR_AST_TYPE_QUALIFIER_ATOMIC:
                    REQUIRE_OK(kefir_json_output_string(json, "atomic"));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected type qualifier");
            }
            break;

        case KEFIR_AST_STORAGE_CLASS_SPECIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "storage_class"));
            REQUIRE_OK(kefir_json_output_object_key(json, "storage"));
            switch (specifier->storage_class) {
                case KEFIR_AST_STORAGE_SPECIFIER_TYPEDEF:
                    REQUIRE_OK(kefir_json_output_string(json, "typedef"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_EXTERN:
                    REQUIRE_OK(kefir_json_output_string(json, "extern"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_STATIC:
                    REQUIRE_OK(kefir_json_output_string(json, "static"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_THREAD_LOCAL:
                    REQUIRE_OK(kefir_json_output_string(json, "thread_local"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_AUTO:
                    REQUIRE_OK(kefir_json_output_string(json, "auto"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_REGISTER:
                    REQUIRE_OK(kefir_json_output_string(json, "register"));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected storage class");
            }
            break;

        case KEFIR_AST_FUNCTION_SPECIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "function_specifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "specifier"));
            switch (specifier->function_specifier) {
                case KEFIR_AST_FUNCTION_SPECIFIER_TYPE_NORETURN:
                    REQUIRE_OK(kefir_json_output_string(json, "noreturn"));
                    break;

                case KEFIR_AST_FUNCTION_SPECIFIER_TYPE_INLINE:
                    REQUIRE_OK(kefir_json_output_string(json, "inline"));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected function specifier");
            }
            break;

        case KEFIR_AST_ALIGNMENT_SPECIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "alignment_specifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "alignment"));
            REQUIRE_OK(kefir_ast_format(json, specifier->alignment_specifier));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected declarator specifier");
    }
    REQUIRE_OK(kefir_json_output_object_end(json));

    return KEFIR_OK;
}

kefir_result_t kefir_ast_format_declarator_specifier_list(
    struct kefir_json_output *json, const struct kefir_ast_declarator_specifier_list *specifiers) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid JSON output"));
    REQUIRE(specifiers != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator specifier list"));

    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_ast_declarator_specifier *specifier = NULL;
    struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(specifiers, &specifier);
    kefir_result_t res = KEFIR_OK;
    for (; res == KEFIR_OK && iter != NULL; res = kefir_ast_declarator_specifier_list_next(&iter, &specifier)) {
        REQUIRE_OK(kefir_ast_format_declarator_specifier(json, specifier));
    }
    REQUIRE_OK(res);
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_type_qualifiers(struct kefir_json_output *json,
                                             const struct kefir_ast_type_qualifier_list *type_qualifiers) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    kefir_ast_type_qualifier_type_t qualifier;
    for (const struct kefir_list_entry *iter = kefir_ast_type_qualifier_list_iter(type_qualifiers, &qualifier);
         iter != NULL; kefir_ast_type_qualifier_list_next(&iter, &qualifier)) {
        switch (qualifier) {
            case KEFIR_AST_TYPE_QUALIFIER_CONST:
                REQUIRE_OK(kefir_json_output_string(json, "const"));
                break;

            case KEFIR_AST_TYPE_QUALIFIER_RESTRICT:
                REQUIRE_OK(kefir_json_output_string(json, "restrict"));
                break;

            case KEFIR_AST_TYPE_QUALIFIER_VOLATILE:
                REQUIRE_OK(kefir_json_output_string(json, "volatile"));
                break;

            case KEFIR_AST_TYPE_QUALIFIER_ATOMIC:
                REQUIRE_OK(kefir_json_output_string(json, "atomic"));
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected type qualifier");
        }
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_format_declarator(struct kefir_json_output *json,
                                           const struct kefir_ast_declarator *declarator) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid JSON output"));
    REQUIRE(declarator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    switch (declarator->klass) {
        case KEFIR_AST_DECLARATOR_IDENTIFIER:
            if (declarator->identifier != NULL) {
                REQUIRE_OK(kefir_json_output_string(json, "identifier"));
                REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
                REQUIRE_OK(kefir_json_output_string(json, declarator->identifier));
            } else {
                REQUIRE_OK(kefir_json_output_string(json, "abstract"));
            }
            break;

        case KEFIR_AST_DECLARATOR_POINTER: {
            REQUIRE_OK(kefir_json_output_string(json, "pointer"));
            REQUIRE_OK(kefir_json_output_object_key(json, "declarator"));
            REQUIRE_OK(kefir_ast_format_declarator(json, declarator->pointer.declarator));
            REQUIRE_OK(kefir_json_output_object_key(json, "type_qualifiers"));
            REQUIRE_OK(format_type_qualifiers(json, &declarator->pointer.type_qualifiers));
        } break;

        case KEFIR_AST_DECLARATOR_ARRAY: {
            REQUIRE_OK(kefir_json_output_string(json, "array"));
            REQUIRE_OK(kefir_json_output_object_key(json, "declarator"));
            REQUIRE_OK(kefir_ast_format_declarator(json, declarator->array.declarator));
            REQUIRE_OK(kefir_json_output_object_key(json, "type"));
            switch (declarator->array.type) {
                case KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED:
                    REQUIRE_OK(kefir_json_output_string(json, "unbounded"));
                    break;

                case KEFIR_AST_DECLARATOR_ARRAY_VLA_UNSPECIFIED:
                    REQUIRE_OK(kefir_json_output_string(json, "vla_unspecified"));
                    break;

                case KEFIR_AST_DECLARATOR_ARRAY_BOUNDED:
                    REQUIRE_OK(kefir_json_output_string(json, "bounded"));
                    REQUIRE_OK(kefir_json_output_object_key(json, "length"));
                    REQUIRE_OK(kefir_ast_format(json, declarator->array.length));
                    break;
            }
            REQUIRE_OK(kefir_json_output_object_key(json, "static"));
            REQUIRE_OK(kefir_json_output_boolean(json, declarator->array.static_array));
            REQUIRE_OK(kefir_json_output_object_key(json, "type_qualifiers"));
            REQUIRE_OK(format_type_qualifiers(json, &declarator->array.type_qualifiers));
        } break;

        case KEFIR_AST_DECLARATOR_FUNCTION:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Function declarator formatter is not implemented yet");
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}
