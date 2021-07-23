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

#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_function_call_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                    const struct kefir_ast_function_call *node,
                                                    struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function call"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

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
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected expression to be a pointer to function"));
    const struct kefir_ast_type *function_type = func_type->referenced_type;

    if (function_type->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS) {
        const struct kefir_list_entry *arg_value_iter = kefir_list_head(&node->arguments);
        const struct kefir_list_entry *arg_type_iter = kefir_list_head(&function_type->function_type.parameters);
        for (; arg_value_iter != NULL && arg_type_iter != NULL;
             kefir_list_next(&arg_type_iter), kefir_list_next(&arg_value_iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, parameter, arg_type_iter->value);
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, arg_value_iter->value);
            REQUIRE_OK(
                kefir_ast_node_assignable(mem, context, arg, kefir_ast_unqualified_type(parameter->adjusted_type)));
        }

        if (arg_type_iter != NULL) {
            ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, parameter, arg_type_iter->value);
            REQUIRE(parameter->adjusted_type->tag == KEFIR_AST_TYPE_VOID &&
                        kefir_ast_type_function_parameter_count(&function_type->function_type) == 1,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function call parameter count does not match prototype"));
        } else if (arg_value_iter != NULL) {
            REQUIRE(function_type->function_type.ellipsis,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function call parameter count does not match prototype"));
        }
    }

    const struct kefir_ast_type *return_type =
        kefir_ast_unqualified_type(func_type->referenced_type->function_type.return_type);
    REQUIRE(return_type->tag != KEFIR_AST_TYPE_ARRAY,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function cannot return array type"));
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type = return_type;
    return KEFIR_OK;
}
