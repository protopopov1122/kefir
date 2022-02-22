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

#include "kefir/ast-translator/jump.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

static kefir_result_t perform_jump(struct kefir_mem *mem, struct kefir_irbuilder_block *builder,
                                   struct kefir_ast_flow_control_structure *original_position,
                                   struct kefir_ast_flow_control_point *target_position,
                                   struct kefir_list *target_parents,
                                   struct kefir_ast_flow_control_structure *common_parent,
                                   const struct kefir_source_location *source_location) {
    for (const struct kefir_list_entry *iter = kefir_list_head(target_parents); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_flow_control_structure *, control_struct, iter->value);
        if (control_struct->type == KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK) {
            REQUIRE(!control_struct->value.block.contains_vla,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, source_location,
                                           "Cannot jump into scope with local VLA variables"));
        }
    }

    struct kefir_ast_flow_control_structure *current_origin_parent = original_position;
    while (current_origin_parent != NULL && current_origin_parent != common_parent) {
        if (current_origin_parent->type == KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK &&
            current_origin_parent->value.block.contains_vla) {
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POPSCOPE, 0));
        }
        current_origin_parent =
            current_origin_parent->parent_point != NULL ? current_origin_parent->parent_point->parent : NULL;
    }

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_JMP, 0));
    REQUIRE_OK(kefir_ast_translator_flow_control_point_reference(mem, target_position, builder->block,
                                                                 KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) - 1));

    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_jump(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                        struct kefir_irbuilder_block *builder,
                                        struct kefir_ast_flow_control_point *original_position,
                                        struct kefir_ast_flow_control_point *target_position,
                                        const struct kefir_source_location *source_location) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(original_position != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid origin AST flow control point"));
    REQUIRE(target_position != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid target AST flow control point"));
    REQUIRE(target_position->parent != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                            "Expected target AST flow control point bound to flow control structure"));

    struct kefir_list target_parents;
    REQUIRE_OK(kefir_list_init(&target_parents));
    struct kefir_ast_flow_control_structure *common_parent = NULL;
    kefir_result_t res = kefir_ast_flow_control_point_common_parent(original_position, target_position, &common_parent);
    REQUIRE_CHAIN(&res, kefir_ast_flow_control_point_parents(mem, target_position, &target_parents, common_parent));
    REQUIRE_CHAIN(&res, perform_jump(mem, builder, original_position->parent, target_position, &target_parents,
                                     common_parent, source_location));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &target_parents);
        return res;
    });
    REQUIRE_OK(kefir_list_free(mem, &target_parents));
    return KEFIR_OK;
}
