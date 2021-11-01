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

kefir_result_t kefir_ast_translate_conditional_statement_node(struct kefir_mem *mem,
                                                              struct kefir_ast_translator_context *context,
                                                              struct kefir_irbuilder_block *builder,
                                                              const struct kefir_ast_conditional_statement *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST conditional statement node"));

    REQUIRE_OK(kefir_ast_translate_expression(mem, node->condition, builder, context));
    const struct kefir_ast_type *condition_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, node->condition->properties.type);
    if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(condition_type)) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_LDTRUNC1, 0));
    }
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BRANCH, 0));
    REQUIRE_OK(kefir_ast_translator_flow_control_point_reference(
        mem, node->base.properties.statement_props.flow_control_statement->value.conditional.thenBranchEnd,
        builder->block, KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) - 1));

    REQUIRE_OK(kefir_ast_translate_statement(mem, node->thenBranch, builder, context));
    if (node->elseBranch != NULL) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_JMP, 0));
        REQUIRE_OK(kefir_ast_translator_flow_control_point_reference(
            mem, node->base.properties.statement_props.flow_control_statement->value.conditional.elseBranchEnd,
            builder->block, KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) - 1));
    }
    REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(
        mem, node->base.properties.statement_props.flow_control_statement->value.conditional.thenBranchEnd,
        KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));

    if (node->elseBranch != NULL) {
        REQUIRE_OK(kefir_ast_translate_statement(mem, node->elseBranch, builder, context));
        REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(
            mem, node->base.properties.statement_props.flow_control_statement->value.conditional.elseBranchEnd,
            KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));
    }

    return KEFIR_OK;
}
