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

#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast/downcast.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

static kefir_result_t implicit_function_declaration(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                    struct kefir_ast_node_base *function) {
    if (function->klass->type == KEFIR_AST_IDENTIFIER) {
        struct kefir_ast_identifier *identifier = NULL;
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;

        REQUIRE_OK(kefir_ast_downcast_identifier(function, &identifier, false));
        kefir_result_t res = context->resolve_ordinary_identifier(context, identifier->identifier, &scoped_id);
        if (res == KEFIR_NOT_FOUND) {
            struct kefir_ast_function_type *implicit_function = NULL;
            const struct kefir_ast_type *implicit_function_type =
                kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_signed_int(), &implicit_function);
            REQUIRE(implicit_function_type != NULL,
                    KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST function type"));
            REQUIRE_OK(context->define_identifier(mem, context, true, identifier->identifier, implicit_function_type,
                                                  KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                  KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL,
                                                  &function->source_location, &scoped_id));
        } else {
            REQUIRE_OK(res);
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_function_call_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                    const struct kefir_ast_function_call *node,
                                                    struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST function call"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    if (context->configuration->analysis.implicit_function_declaration) {
        REQUIRE_OK(implicit_function_declaration(mem, context, node->function));
    }

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->function));
    const struct kefir_list_entry *iter;
    for (iter = kefir_list_head(&node->arguments); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, arg));
    }

    const struct kefir_ast_type *func_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->function->properties.type);
    REQUIRE(
        func_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER && func_type->referenced_type->tag == KEFIR_AST_TYPE_FUNCTION,
        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->function->source_location,
                               "Expected expression to be a pointer to function"));
    const struct kefir_ast_type *function_type = func_type->referenced_type;

    if (function_type->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS) {
        const struct kefir_list_entry *arg_value_iter = kefir_list_head(&node->arguments);
        const struct kefir_list_entry *arg_type_iter = kefir_list_head(&function_type->function_type.parameters);
        for (; arg_value_iter != NULL && arg_type_iter != NULL;
             kefir_list_next(&arg_type_iter), kefir_list_next(&arg_value_iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, parameter, arg_type_iter->value);
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, arg_value_iter->value);

            kefir_result_t res;
            REQUIRE_MATCH_OK(
                &res,
                kefir_ast_node_assignable(mem, context, arg, kefir_ast_unqualified_type(parameter->adjusted_type)),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &arg->source_location,
                                       "Expression value shall be assignable to function parameter type"));
            REQUIRE_OK(res);
        }

        if (arg_type_iter != NULL) {
            ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, parameter, arg_type_iter->value);
            REQUIRE(parameter->adjusted_type->tag == KEFIR_AST_TYPE_VOID &&
                        kefir_ast_type_function_parameter_count(&function_type->function_type) == 1,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                           "Function call parameter count does not match prototype"));
        } else if (arg_value_iter != NULL) {
            REQUIRE(function_type->function_type.ellipsis,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                           "Function call parameter count does not match prototype"));
        }
    }

    const struct kefir_ast_type *return_type =
        kefir_ast_unqualified_type(func_type->referenced_type->function_type.return_type);
    REQUIRE(return_type->tag != KEFIR_AST_TYPE_ARRAY,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->function->source_location,
                                   "Function cannot return array type"));
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type = return_type;

    if (KEFIR_AST_TYPE_IS_AGGREGATE_TYPE(return_type)) {
        REQUIRE_OK(context->allocate_temporary_value(mem, context, return_type, NULL, &base->source_location,
                                                     &base->properties.expression_props.temporary));
    }
    return KEFIR_OK;
}
