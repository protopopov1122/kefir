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

#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/util.h"
#include "kefir/ast-translator/misc.h"
#include "kefir/ast/downcast.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_statement_expression_node(struct kefir_mem *mem,
                                                             struct kefir_ast_translator_context *context,
                                                             struct kefir_irbuilder_block *builder,
                                                             const struct kefir_ast_statement_expression *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST statement expression node"));

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->block_items); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item, iter->value);

        if (item->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT) {
            REQUIRE_OK(kefir_ast_translate_statement(mem, item, builder, context));
        } else if (item->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION ||
                   item->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR) {
            REQUIRE_OK(kefir_ast_translate_declaration(mem, item, builder, context));
        } else {
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected compound statement item");
        }
    }

    if (node->result != NULL) {
        kefir_result_t res;
        struct kefir_ast_expression_statement *expr_statement = NULL;

        REQUIRE_MATCH_OK(&res, kefir_ast_downcast_expression_statement(node->result, &expr_statement, false),
                         KEFIR_SET_ERROR(KEFIR_INVALID_STATE,
                                         "Last statement of statement expression shall be an expression statement"));
        REQUIRE_OK(kefir_ast_translate_expression(mem, expr_statement->expression, builder, context));
    }

    if (node->base.properties.expression_props.flow_control_statement->value.block.contains_vla) {
        const struct kefir_ast_flow_control_data_element *vla_element = NULL;
        REQUIRE_OK(kefir_ast_flow_control_structure_data_element_head(
            &node->base.properties.expression_props.flow_control_statement->value.block.data_elements, &vla_element));

        REQUIRE_OK(kefir_ast_translator_resolve_vla_element(mem, context, builder, vla_element->identifier));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LOAD64, 0));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POPSCOPE, 0));
    }
    return KEFIR_OK;
}
