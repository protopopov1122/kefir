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

static kefir_result_t resolve_loop(const struct kefir_ast_flow_control_structure *stmt, void *payload,
                                   kefir_bool_t *result) {
    UNUSED(payload);
    REQUIRE(stmt != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control statement"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to boolean"));

    switch (stmt->type) {
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK:
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_IF:
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_SWITCH:
            *result = false;
            break;

        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_FOR:
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_WHILE:
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_DO:
            *result = true;
            break;
    }

    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_continue_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                         const struct kefir_ast_continue_statement *node,
                                                         struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST continue statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    REQUIRE(context->flow_control_tree != NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Continue statement is not allowed in current context"));

    struct kefir_ast_flow_control_structure *flow_control_stmt = NULL;
    kefir_result_t res =
        kefir_ast_flow_control_tree_traverse(context->flow_control_tree, resolve_loop, NULL, &flow_control_stmt);
    if (res == KEFIR_NOT_FOUND) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                      "Continue statement is not allowed outside of while/do/for loop bodies");
    } else {
        REQUIRE_OK(res);
    }
    base->properties.statement_props.target_flow_control_point = flow_control_stmt->value.loop.continuation;
    REQUIRE_OK(
        context->current_flow_control_point(mem, context, &base->properties.statement_props.origin_flow_control_point));
    return KEFIR_OK;
}
