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

#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct visitor_param {
    const struct kefir_ast_context *context;
    kefir_bool_t *constant;
};

static kefir_result_t visit_non_const_ref(const struct kefir_ast_visitor *visitor,
                                          const struct kefir_ast_node_base *node, void *payload) {
    UNUSED(visitor);
    UNUSED(node);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct visitor_param *, param, payload);
    *param->constant = false;
    return KEFIR_OK;
}

static kefir_result_t visit_identifier(const struct kefir_ast_visitor *visitor, const struct kefir_ast_identifier *node,
                                       void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST identifier"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct visitor_param *, param, payload);

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE_OK(param->context->resolve_ordinary_identifier(param->context, node->identifier, &scoped_id));
    switch (scoped_id->klass) {
        case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT: {
            *param->constant = scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC ||
                               scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN;
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
            *param->constant = true;
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:
        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG:
        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION:
        case KEFIR_AST_SCOPE_IDENTIFIER_LABEL:
            *param->constant = false;
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t visit_struct_member(const struct kefir_ast_visitor *visitor,
                                          const struct kefir_ast_struct_member *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST struct member node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    REQUIRE_OK(KEFIR_AST_NODE_VISIT(visitor, node->structure, payload));
    return KEFIR_OK;
}

static kefir_result_t visit_array_subscript(const struct kefir_ast_visitor *visitor,
                                            const struct kefir_ast_array_subscript *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST array subscript node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct visitor_param *, param, payload);

    const struct kefir_ast_type *array_type = kefir_ast_type_lvalue_conversion(node->array->properties.type);
    if (array_type->tag == KEFIR_AST_TYPE_ARRAY || array_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        REQUIRE_OK(KEFIR_AST_NODE_VISIT(visitor, node->array, payload));
        *param->constant = *param->constant && node->subscript->properties.expression_props.constant_expression;
    } else {
        REQUIRE_OK(KEFIR_AST_NODE_VISIT(visitor, node->subscript, payload));
        *param->constant = *param->constant && node->array->properties.expression_props.constant_expression;
    }
    return KEFIR_OK;
}

static kefir_result_t visit_struct_indirect_member(const struct kefir_ast_visitor *visitor,
                                                   const struct kefir_ast_struct_member *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST struct member node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct visitor_param *, param, payload);

    *param->constant = *param->constant && node->structure->properties.expression_props.constant_expression;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_node_is_lvalue_reference_constant(const struct kefir_ast_context *context,
                                                           const struct kefir_ast_node_base *node,
                                                           kefir_bool_t *constant) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node"));
    REQUIRE(constant != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to boolean"));

    struct visitor_param param = {.context = context, .constant = constant};
    *constant = true;
    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, visit_non_const_ref));
    visitor.identifier = visit_identifier;
    visitor.struct_member = visit_struct_member;
    visitor.array_subscript = visit_array_subscript;
    visitor.struct_indirect_member = visit_struct_indirect_member;
    REQUIRE_OK(KEFIR_AST_NODE_VISIT(&visitor, node, &param));
    return KEFIR_OK;
}
