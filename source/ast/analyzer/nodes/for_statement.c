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

kefir_result_t kefir_ast_analyze_for_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                    const struct kefir_ast_for_statement *node,
                                                    struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST for statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    REQUIRE(context->flow_control_tree != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to use for statement in current context"));
    REQUIRE_OK(kefir_ast_flow_control_tree_push(mem, context->flow_control_tree, KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR,
                                                &base->properties.statement_props.flow_control_statement));

    base->properties.statement_props.flow_control_statement->value.loop.continuation =
        kefir_ast_flow_control_point_alloc(mem);
    REQUIRE(base->properties.statement_props.flow_control_statement->value.loop.continuation != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST flow control point"));

    base->properties.statement_props.flow_control_statement->value.loop.end = kefir_ast_flow_control_point_alloc(mem);
    REQUIRE(base->properties.statement_props.flow_control_statement->value.loop.end != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST flow control point"));

    REQUIRE_OK(context->push_block(mem, context));

    if (node->init != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->init));
        if (node->init->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION) {
            REQUIRE(node->init->properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO ||
                        node->init->properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                    KEFIR_SET_ERROR(
                        KEFIR_MALFORMED_ARG,
                        "Expected the first clause of for statement to declare only auto or register identifiers"));
        } else {
            REQUIRE(
                node->init->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Expected the first clause of for statement to be either declaration or expression"));
        }
    }

    if (node->controlling_expr != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->controlling_expr));
        REQUIRE(node->controlling_expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Expected the second expression of for statement to be an expression"));
        const struct kefir_ast_type *condition_type =
            KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->controlling_expr->properties.type);
        REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(condition_type),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected while statement condition to be scalar expression"));
    }

    if (node->tail != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->tail));
        REQUIRE(
            node->tail->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected the third expression of for statement to be an expression"));
    }

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->body));
    REQUIRE(node->body->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected for statement body to be a statement"));

    REQUIRE_OK(context->pop_block(mem, context));
    REQUIRE_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    return KEFIR_OK;
}
