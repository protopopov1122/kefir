/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"
#include "kefir/ir/type.h"

struct designator_param {
    struct kefir_mem *mem;
    const struct kefir_ast_context *context;
    const struct kefir_ast_node_base *base;
    struct kefir_ast_designator *designator;
};

static kefir_result_t visit_non_designator(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_node_base *base, void *payload) {
    UNUSED(visitor);
    UNUSED(base);
    UNUSED(payload);
    return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &base->source_location, "Expected a member designator");
}

static kefir_result_t visit_identifier(const struct kefir_ast_visitor *visitor,
                                       const struct kefir_ast_identifier *identifier, void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST visitor"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST identifier"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct designator_param *, param, payload);

    struct kefir_ast_designator *designator =
        kefir_ast_new_member_designator(param->mem, param->context->symbols, identifier->identifier, param->designator);
    REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate member designator"));
    param->designator = designator;
    return KEFIR_OK;
}

static kefir_result_t visit_struct_member(const struct kefir_ast_visitor *visitor,
                                          const struct kefir_ast_struct_member *member, void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST visitor"));
    REQUIRE(member != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST struct member"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct designator_param *, param, payload);

    REQUIRE_OK(KEFIR_AST_NODE_VISIT(visitor, member->structure, payload));

    struct kefir_ast_designator *designator =
        kefir_ast_new_member_designator(param->mem, param->context->symbols, member->member, param->designator);
    REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate member designator"));
    param->designator = designator;
    return KEFIR_OK;
}

static kefir_result_t visit_array_subscript(const struct kefir_ast_visitor *visitor,
                                            const struct kefir_ast_array_subscript *subscript, void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST visitor"));
    REQUIRE(subscript != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST array subscript"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct designator_param *, param, payload);

    REQUIRE_OK(KEFIR_AST_NODE_VISIT(visitor, subscript->array, payload));

    struct kefir_ast_constant_expression_value value;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(param->mem, param->context, subscript->subscript, &value));

    struct kefir_ast_designator *designator =
        kefir_ast_new_index_designator(param->mem, value.integer, param->designator);
    REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate member designator"));
    param->designator = designator;
    return KEFIR_OK;
}

static kefir_result_t build_member_designator(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                              const struct kefir_ast_node_base *base,
                                              struct kefir_ast_designator **designator) {

    struct designator_param param = {.mem = mem, .context = context, .base = base, .designator = NULL};
    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, visit_non_designator));
    visitor.identifier = visit_identifier;
    visitor.struct_member = visit_struct_member;
    visitor.array_subscript = visit_array_subscript;
    kefir_result_t res = KEFIR_AST_NODE_VISIT(&visitor, base, &param);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_designator_free(mem, param.designator);
        return res;
    });
    *designator = param.designator;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_evaluate_builtin_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                               const struct kefir_ast_builtin *node,
                                               struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant node"));
    REQUIRE(value != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Expected constant expression AST node"));
    REQUIRE(node->base.properties.expression_props.constant_expression,
            KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->base.source_location,
                                   "Expected constant expression AST node"));

    const struct kefir_list_entry *iter = kefir_list_head(&node->arguments);
    switch (node->builtin) {
        case KEFIR_AST_BUILTIN_OFFSETOF: {
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, offset_base, iter->value);
            kefir_list_next(&iter);
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, member_designator, iter->value);

            struct kefir_ast_designator *designator = NULL;
            REQUIRE_OK(build_member_designator(mem, context, member_designator, &designator));

            kefir_ast_target_environment_opaque_type_t opaque_type;
            kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, context->target_env,
                                                                       offset_base->properties.type, &opaque_type);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_designator_free(mem, designator);
                return res;
            });

            struct kefir_ast_target_environment_object_info objinfo;
            res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, context->target_env, opaque_type, designator, &objinfo);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_designator_free(mem, designator);
                KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type);
                return res;
            });
            res = KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_designator_free(mem, designator);
                return res;
            });
            REQUIRE_OK(kefir_ast_designator_free(mem, designator));

            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
            value->integer = objinfo.relative_offset;
        } break;

        case KEFIR_AST_BUILTIN_VA_START:
        case KEFIR_AST_BUILTIN_VA_END:
        case KEFIR_AST_BUILTIN_VA_ARG:
        case KEFIR_AST_BUILTIN_VA_COPY:
        case KEFIR_AST_BUILTIN_ALLOCA:
        case KEFIR_AST_BUILTIN_ALLOCA_WITH_ALIGN:
        case KEFIR_AST_BUILTIN_ALLOCA_WITH_ALIGN_AND_MAX:
            return KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &node->base.source_location,
                                          "Builtin operation is not a constant expression");
    }

    return KEFIR_OK;
}
