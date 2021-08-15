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

static kefir_result_t visit_cast_operator(const struct kefir_ast_visitor *visitor,
                                          const struct kefir_ast_cast_operator *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST cast operator node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "cast_operator"));
    REQUIRE_OK(kefir_json_output_object_key(json, "type_name"));
    REQUIRE_OK(kefir_ast_format(json, KEFIR_AST_NODE_BASE(node->type_name)));
    REQUIRE_OK(kefir_json_output_object_key(json, "expression"));
    REQUIRE_OK(kefir_ast_format(json, node->expr));
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

static kefir_result_t visit_type_name(const struct kefir_ast_visitor *visitor, const struct kefir_ast_type_name *node,
                                      void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type name node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "type_name"));
    REQUIRE_OK(kefir_json_output_object_key(json, "specifiers"));
    REQUIRE_OK(kefir_ast_format_declarator_specifier_list(json, &node->type_decl.specifiers));
    REQUIRE_OK(kefir_json_output_object_key(json, "declarator"));
    REQUIRE_OK(kefir_ast_format_declarator(json, node->type_decl.declarator));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_declaration_list(const struct kefir_ast_visitor *visitor,
                                             const struct kefir_ast_declaration_list *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declaration list node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "declaration_list"));
    REQUIRE_OK(kefir_json_output_object_key(json, "specifiers"));
    REQUIRE_OK(kefir_ast_format_declarator_specifier_list(json, &node->specifiers));
    REQUIRE_OK(kefir_json_output_object_key(json, "declarations"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->declarations); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl, iter->value);
        REQUIRE_OK(kefir_ast_format(json, decl));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_declaration(const struct kefir_ast_visitor *visitor,
                                        const struct kefir_ast_declaration *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declaration node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "declaration"));
    REQUIRE_OK(kefir_json_output_object_key(json, "declarator"));
    REQUIRE_OK(kefir_ast_format_declarator(json, node->declarator));
    REQUIRE_OK(kefir_json_output_object_key(json, "initializer"));
    if (node->initializer != NULL) {
        REQUIRE_OK(kefir_ast_format_initializer(json, node->initializer));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_static_assertion(const struct kefir_ast_visitor *visitor,
                                             const struct kefir_ast_static_assertion *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST static assertion node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "static_assertion"));
    REQUIRE_OK(kefir_json_output_object_key(json, "assertion"));
    REQUIRE_OK(kefir_ast_format(json, node->condition));
    REQUIRE_OK(kefir_json_output_object_key(json, "string_literal"));
    REQUIRE_OK(kefir_ast_format(json, KEFIR_AST_NODE_BASE(node->string)));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_generic_selection(const struct kefir_ast_visitor *visitor,
                                              const struct kefir_ast_generic_selection *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST generic selection node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "generic_selection"));
    REQUIRE_OK(kefir_json_output_object_key(json, "control_expression"));
    REQUIRE_OK(kefir_ast_format(json, node->control));
    REQUIRE_OK(kefir_json_output_object_key(json, "associations"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->associations); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc, iter->value);

        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "type_name"));
        REQUIRE_OK(kefir_ast_format(json, KEFIR_AST_NODE_BASE(assoc->type_name)));
        REQUIRE_OK(kefir_json_output_object_key(json, "expression"));
        REQUIRE_OK(kefir_ast_format(json, assoc->expr));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "default_association"));
    if (node->default_assoc != NULL) {
        REQUIRE_OK(kefir_ast_format(json, node->default_assoc));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_compound_literal(const struct kefir_ast_visitor *visitor,
                                             const struct kefir_ast_compound_literal *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST compound literal node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "compound_literal"));
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    REQUIRE_OK(kefir_ast_format(json, KEFIR_AST_NODE_BASE(node->type_name)));
    REQUIRE_OK(kefir_json_output_object_key(json, "initializer"));
    REQUIRE_OK(kefir_ast_format_initializer(json, node->initializer));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_expression_statement(const struct kefir_ast_visitor *visitor,
                                                 const struct kefir_ast_expression_statement *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST expression statement node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "expression_statement"));
    REQUIRE_OK(kefir_json_output_object_key(json, "expression"));
    if (node->expression != NULL) {
        REQUIRE_OK(kefir_ast_format(json, node->expression));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_compound_statement(const struct kefir_ast_visitor *visitor,
                                               const struct kefir_ast_compound_statement *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST compound statement node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "compound_statement"));
    REQUIRE_OK(kefir_json_output_object_key(json, "items"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->block_items); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item, iter->value);
        REQUIRE_OK(kefir_ast_format(json, item));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_labeled_statement(const struct kefir_ast_visitor *visitor,
                                              const struct kefir_ast_labeled_statement *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST labeled statement node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "labeled_statement"));
    REQUIRE_OK(kefir_json_output_object_key(json, "label"));
    REQUIRE_OK(kefir_json_output_string(json, node->label));
    REQUIRE_OK(kefir_json_output_object_key(json, "statement"));
    REQUIRE_OK(kefir_ast_format(json, node->statement));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_case_statement(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_case_statement *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST case statement node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "case_statement"));
    REQUIRE_OK(kefir_json_output_object_key(json, "expression"));
    if (node->expression != NULL) {
        REQUIRE_OK(kefir_ast_format(json, node->expression));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "statement"));
    REQUIRE_OK(kefir_ast_format(json, node->statement));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_if_statement(const struct kefir_ast_visitor *visitor,
                                         const struct kefir_ast_conditional_statement *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST conditional statement node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "if_statement"));
    REQUIRE_OK(kefir_json_output_object_key(json, "expression"));
    REQUIRE_OK(kefir_ast_format(json, node->condition));
    REQUIRE_OK(kefir_json_output_object_key(json, "thenBranch"));
    REQUIRE_OK(kefir_ast_format(json, node->thenBranch));
    REQUIRE_OK(kefir_json_output_object_key(json, "elseBranch"));
    if (node->elseBranch != NULL) {
        REQUIRE_OK(kefir_ast_format(json, node->elseBranch));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_switch_statement(const struct kefir_ast_visitor *visitor,
                                             const struct kefir_ast_switch_statement *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST switch statement node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "switch_statement"));
    REQUIRE_OK(kefir_json_output_object_key(json, "expression"));
    REQUIRE_OK(kefir_ast_format(json, node->expression));
    REQUIRE_OK(kefir_json_output_object_key(json, "statement"));
    REQUIRE_OK(kefir_ast_format(json, node->statement));
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
    visitor.cast_operator = visit_cast_operator;
    visitor.binary_operation = visit_binary_operation;
    visitor.conditional_operator = visit_conditional_operator;
    visitor.assignment_operator = visit_assignment_operator;
    visitor.comma_operator = visit_comma_operator;
    visitor.type_name = visit_type_name;
    visitor.declaration_list = visit_declaration_list;
    visitor.declaration = visit_declaration;
    visitor.static_assertion = visit_static_assertion;
    visitor.generic_selection = visit_generic_selection;
    visitor.compound_literal = visit_compound_literal;
    visitor.expression_statement = visit_expression_statement;
    visitor.compound_statement = visit_compound_statement;
    visitor.labeled_statement = visit_labeled_statement;
    visitor.case_statement = visit_case_statement;
    visitor.conditional_statement = visit_if_statement;
    visitor.switch_statement = visit_switch_statement;
    REQUIRE_OK(node->klass->visit(node, &visitor, json));
    return KEFIR_OK;
}
