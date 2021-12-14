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

#include "kefir/ast/constant_expression_impl.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

struct visitor_param {
    struct kefir_mem *mem;
    const struct kefir_ast_context *context;
    struct kefir_ast_constant_expression_pointer *pointer;
};

static kefir_result_t non_const_expr(const struct kefir_ast_visitor *visitor, const struct kefir_ast_node_base *node,
                                     void *payload) {
    UNUSED(visitor);
    UNUSED(node);
    UNUSED(payload);
    return KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->source_location,
                                  "Provided expression is not a constant lvalue");
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
            REQUIRE(
                scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC ||
                    scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->base.source_location, "Not a constant expression"));
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:
        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG:
        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION:
        case KEFIR_AST_SCOPE_IDENTIFIER_LABEL:
            return KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->base.source_location, "Not a constant expression");
    }

    param->pointer->type = KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER;
    param->pointer->base.literal = node->identifier;
    param->pointer->offset = 0;
    param->pointer->pointer_node = KEFIR_AST_NODE_BASE(node);
    param->pointer->scoped_id = scoped_id;
    return KEFIR_OK;
}

static kefir_result_t visit_structure_member(const struct kefir_ast_visitor *visitor,
                                             const struct kefir_ast_struct_member *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST struct member"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct visitor_param *, param, payload);

    struct kefir_ast_constant_expression_pointer base_pointer;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate_lvalue_reference(param->mem, param->context,
                                                                             node->structure, &base_pointer));

    struct kefir_ast_designator designator = {
        .type = KEFIR_AST_DESIGNATOR_MEMBER, .member = node->member, .next = NULL};

    struct kefir_ast_target_environment_object_info object_info;
    kefir_ast_target_environment_opaque_type_t opaque_type;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(param->mem, param->context->target_env,
                                                     node->structure->properties.type, &opaque_type));
    kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(param->mem, param->context->target_env, opaque_type,
                                                                  &designator, &object_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(param->mem, param->context->target_env, opaque_type);
        return res;
    });
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(param->mem, param->context->target_env, opaque_type));

    *param->pointer = base_pointer;
    param->pointer->offset += object_info.relative_offset;
    param->pointer->pointer_node = KEFIR_AST_NODE_BASE(node);

    return KEFIR_OK;
}

static kefir_result_t visit_array_subscript(const struct kefir_ast_visitor *visitor,
                                            const struct kefir_ast_array_subscript *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST array subscript node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct visitor_param *, param, payload);

    const struct kefir_ast_type *array_type = kefir_ast_type_lvalue_conversion(node->array->properties.type);
    struct kefir_ast_node_base *array = node->array;
    struct kefir_ast_node_base *subscript = node->subscript;
    if (array_type->tag != KEFIR_AST_TYPE_ARRAY && array_type->tag != KEFIR_AST_TYPE_SCALAR_POINTER) {
        array = node->subscript;
        subscript = node->array;
        array_type = kefir_ast_type_lvalue_conversion(node->subscript->properties.type);
    }

    struct kefir_ast_constant_expression_pointer base_pointer;
    if (array_type->tag == KEFIR_AST_TYPE_ARRAY) {
        REQUIRE_OK(kefir_ast_constant_expression_value_evaluate_lvalue_reference(param->mem, param->context, array,
                                                                                 &base_pointer));
    } else {
        REQUIRE(
            array_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &array->source_location, "Expected either array or pointer"));
        struct kefir_ast_constant_expression_value base_expr;
        REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(param->mem, param->context, array, &base_expr));
        REQUIRE(base_expr.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &array->source_location,
                                       "Expected constant expression to yield an address"));
        base_pointer = base_expr.pointer;

        array_type =
            kefir_ast_type_unbounded_array(param->mem, param->context->type_bundle, array_type->referenced_type, NULL);
    }

    struct kefir_ast_constant_expression_value subscript_value;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(param->mem, param->context, subscript, &subscript_value));
    REQUIRE(subscript_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->subscript->source_location,
                                   "Expected constant array subscript to have integral type"));

    struct kefir_ast_designator designator = {
        .type = KEFIR_AST_DESIGNATOR_SUBSCRIPT, .index = subscript_value.integer, .next = NULL};

    struct kefir_ast_target_environment_object_info object_info;
    kefir_ast_target_environment_opaque_type_t opaque_type;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(param->mem, param->context->target_env, array_type, &opaque_type));
    kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(param->mem, param->context->target_env, opaque_type,
                                                                  &designator, &object_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(param->mem, param->context->target_env, opaque_type);
        return res;
    });
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(param->mem, param->context->target_env, opaque_type));

    *param->pointer = base_pointer;
    param->pointer->offset += object_info.relative_offset;
    param->pointer->pointer_node = KEFIR_AST_NODE_BASE(node);

    return KEFIR_OK;
}

static kefir_result_t visit_struct_indirect_member(const struct kefir_ast_visitor *visitor,
                                                   const struct kefir_ast_struct_member *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST struct member node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct visitor_param *, param, payload);

    struct kefir_ast_constant_expression_value base_expr;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(param->mem, param->context, node->structure, &base_expr));
    REQUIRE(base_expr.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->structure->source_location,
                                   "Expected constant expression to yield an address"));

    struct kefir_ast_designator designator = {
        .type = KEFIR_AST_DESIGNATOR_MEMBER, .member = node->member, .next = NULL};

    struct kefir_ast_target_environment_object_info object_info;
    kefir_ast_target_environment_opaque_type_t opaque_type;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(param->mem, param->context->target_env,
                                                     node->structure->properties.type->referenced_type, &opaque_type));
    kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(param->mem, param->context->target_env, opaque_type,
                                                                  &designator, &object_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(param->mem, param->context->target_env, opaque_type);
        return res;
    });
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(param->mem, param->context->target_env, opaque_type));

    *param->pointer = base_expr.pointer;
    param->pointer->offset += object_info.relative_offset;
    param->pointer->pointer_node = KEFIR_AST_NODE_BASE(node);

    return KEFIR_OK;
}

kefir_result_t kefir_ast_constant_expression_value_evaluate_lvalue_reference(
    struct kefir_mem *mem, const struct kefir_ast_context *context, const struct kefir_ast_node_base *node,
    struct kefir_ast_constant_expression_pointer *pointer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node"));
    REQUIRE(pointer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context pointer expression"));

    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, non_const_expr));
    visitor.identifier = visit_identifier;
    visitor.struct_member = visit_structure_member;
    visitor.array_subscript = visit_array_subscript;
    visitor.struct_indirect_member = visit_struct_indirect_member;

    struct visitor_param param = {.mem = mem, .context = context, .pointer = pointer};
    REQUIRE_OK(KEFIR_AST_NODE_VISIT(&visitor, node, &param));
    return KEFIR_OK;
}
