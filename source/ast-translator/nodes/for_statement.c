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
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast-translator/util.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

kefir_result_t kefir_ast_translate_for_statement_node(struct kefir_mem *mem,
                                                      struct kefir_ast_translator_context *context,
                                                      struct kefir_irbuilder_block *builder,
                                                      const struct kefir_ast_for_statement *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST for statement node"));

    if (node->init != NULL && (node->init->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION ||
                               node->init->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR)) {
        REQUIRE_OK(kefir_ast_translate_declaration(mem, node->init, builder, context));
    } else if (node->init != NULL) {
        REQUIRE(node->init->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                KEFIR_SET_ERROR(KEFIR_INVALID_STATE,
                                "Expected AST for statement first clause to be either declaration or expression"));

        const struct kefir_ast_type *clause1_type = kefir_ast_translator_normalize_type(node->init->properties.type);
        REQUIRE(clause1_type != NULL,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unable to obtain normalized expression type"));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->init, builder, context));
        if (clause1_type->tag != KEFIR_AST_TYPE_VOID) {
            if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(clause1_type)) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
            }
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
        }
    }

    struct kefir_ast_flow_control_structure *flow_control_stmt =
        node->base.properties.statement_props.flow_control_statement;

    kefir_size_t begin = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);

    if (node->controlling_expr != NULL) {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->controlling_expr, builder, context));
        const struct kefir_ast_type *controlling_expr_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(
            mem, context->ast_context->type_bundle, node->controlling_expr->properties.type);
        if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(controlling_expr_type)) {
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDTRUNC1, 0));
        }
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BRANCH, 0));
        REQUIRE_OK(kefir_ast_translator_flow_control_point_reference(
            mem, flow_control_stmt->value.loop.end, builder->block, KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) - 1));
    }

    REQUIRE_OK(kefir_ast_translate_statement(mem, node->body, builder, context));

    REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(mem, flow_control_stmt->value.loop.continuation,
                                                               KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));
    if (node->tail != NULL) {
        const struct kefir_ast_type *tail_type = kefir_ast_translator_normalize_type(node->tail->properties.type);
        REQUIRE(tail_type != NULL,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unable to obtain normalized expression type"));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->tail, builder, context));
        if (tail_type->tag != KEFIR_AST_TYPE_VOID) {
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
        }
    }

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_JMP, begin));
    REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(mem, flow_control_stmt->value.loop.end,
                                                               KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));

    return KEFIR_OK;
}
