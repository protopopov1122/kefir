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
#include "kefir/ast-translator/jump.h"
#include "kefir/core/source_error.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_goto_statement_node(struct kefir_mem *mem,
                                                       struct kefir_ast_translator_context *context,
                                                       struct kefir_irbuilder_block *builder,
                                                       const struct kefir_ast_goto_statement *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST goto statement node"));

    if (node->base.klass->type == KEFIR_AST_GOTO_STATEMENT) {
        REQUIRE_OK(kefir_ast_translate_jump(
            mem, context, builder, node->base.properties.statement_props.origin_flow_control_point,
            node->base.properties.statement_props.target_flow_control_point, &node->base.source_location));
    } else {
        struct kefir_ast_flow_control_structure *goto_parent =
            node->base.properties.statement_props.flow_control_statement;
        while (goto_parent != NULL) {
            if (goto_parent->type == KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK) {
                REQUIRE(!goto_parent->value.block.contains_vla,
                        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                               "None of blocks enclosing the address goto can contain VLAs"));
            }
            goto_parent = goto_parent->parent_point != NULL ? goto_parent->parent_point->parent : NULL;
        }

        REQUIRE_OK(kefir_ast_translate_expression(mem, node->target, builder, context));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IJMP, 0));
    }
    return KEFIR_OK;
}
