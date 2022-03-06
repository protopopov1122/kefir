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
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"
#include "kefir/ast/downcast.h"

kefir_result_t kefir_ast_analyze_statement_expression_node(struct kefir_mem *mem,
                                                           const struct kefir_ast_context *context,
                                                           const struct kefir_ast_statement_expression *node,
                                                           struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST compound statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;

    REQUIRE(context->flow_control_tree != NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Unable to use compound statement in current context"));
    REQUIRE_OK(kefir_ast_flow_control_tree_push(mem, context->flow_control_tree, KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK,
                                                &base->properties.expression_props.flow_control_statement));

    REQUIRE_OK(context->push_block(mem, context));
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->block_items); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, item));
        REQUIRE(item->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT ||
                    item->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION ||
                    item->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &item->source_location,
                                       "Compound statement items shall be either statements or declarations"));
    }

    if (node->result != NULL) {
        kefir_result_t res;
        struct kefir_ast_expression_statement *expr_statement = NULL;

        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->result));
        REQUIRE_MATCH_OK(
            &res, kefir_ast_downcast_expression_statement(node->result, &expr_statement, false),
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->result->source_location,
                                   "Last statement of statement expression shall be an expression statement"));

        base->properties.type =
            KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, expr_statement->expression->properties.type);
    } else {
        base->properties.type = kefir_ast_type_void();
    }

    REQUIRE_OK(context->pop_block(mem, context));
    REQUIRE_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    return KEFIR_OK;
}
