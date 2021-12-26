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

#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

kefir_result_t kefir_ast_analyze_return_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_return_statement *node,
                                                       struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST return statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    if (node->expression != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->expression));
        REQUIRE(node->expression->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->expression->source_location,
                                       "Return statement should return an expression"));
    }

    REQUIRE(context->surrounding_function != NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Return statement should appear only in the context of a function"));
    const struct kefir_ast_type *function_return_type =
        kefir_ast_unqualified_type(context->surrounding_function->function.type->function_type.return_type);
    if (function_return_type->tag == KEFIR_AST_TYPE_VOID) {
        REQUIRE(node->expression == NULL,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->expression->source_location,
                                       "Return statement with expression shall appear only in non-void function"));
    } else {
        REQUIRE(node->expression != NULL,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                       "Return statement with no expression shall appear only in void function"));
        const struct kefir_ast_type *value_type =
            KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->expression->properties.type);

        kefir_result_t res;
        REQUIRE_MATCH_OK(&res,
                         kefir_ast_type_assignable(mem, context, value_type,
                                                   node->expression->properties.expression_props.constant_expression,
                                                   function_return_type),
                         KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                                "Returned value shall be assignable to the function return type"));
    }
    base->properties.statement_props.return_type = function_return_type;
    return KEFIR_OK;
}
