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

#include "kefir/ast-translator/flow_control.h"
#include "kefir/test/unit_test.h"

DEFINE_CASE(ast_translator_flow_control_point1, "AST Translator - flow control points #1") {
    struct kefir_ast_flow_control_point *point1 = kefir_ast_flow_control_point_alloc(&kft_mem);
    ASSERT(point1 != NULL);

    struct kefir_ast_translator_flow_control_point *translator_point1 = NULL;
    ASSERT_OK(kefir_ast_translator_flow_control_point_init(&kft_mem, point1, &translator_point1));
    ASSERT(translator_point1 != NULL);
    ASSERT(!translator_point1->resolved);

    struct kefir_irblock block;
    ASSERT_OK(kefir_irblock_alloc(&kft_mem, 1024, &block));
    ASSERT_OK(kefir_irblock_appendu64(&block, KEFIR_IROPCODE_JMP, 0));
    ASSERT_OK(kefir_irblock_appendu64(&block, KEFIR_IROPCODE_BRANCH, 0));

    ASSERT_OK(kefir_ast_translator_flow_control_point_reference(&kft_mem, point1, &block, 0));
    ASSERT_OK(kefir_ast_translator_flow_control_point_reference(&kft_mem, point1, &block, 1));
    ASSERT(!translator_point1->resolved);
    ASSERT(kefir_list_length(&translator_point1->dependents) == 2);
    ASSERT(kefir_irblock_at(&block, 0)->opcode == KEFIR_IROPCODE_JMP);
    ASSERT(kefir_irblock_at(&block, 0)->arg.u64 == 0);
    ASSERT(kefir_irblock_at(&block, 1)->opcode == KEFIR_IROPCODE_BRANCH);
    ASSERT(kefir_irblock_at(&block, 1)->arg.u64 == 0);

    ASSERT_OK(kefir_irblock_appendu64(&block, KEFIR_IROPCODE_NOP, 0));
    ASSERT_OK(kefir_ast_translator_flow_control_point_resolve(&kft_mem, point1, 2));
    ASSERT(translator_point1->resolved);
    ASSERT(translator_point1->address == 2);

    ASSERT(kefir_irblock_at(&block, 0)->opcode == KEFIR_IROPCODE_JMP);
    ASSERT(kefir_irblock_at(&block, 0)->arg.u64 == 2);
    ASSERT(kefir_irblock_at(&block, 1)->opcode == KEFIR_IROPCODE_BRANCH);
    ASSERT(kefir_irblock_at(&block, 1)->arg.u64 == 2);
    ASSERT(kefir_irblock_at(&block, 2)->opcode == KEFIR_IROPCODE_NOP);
    ASSERT(kefir_irblock_at(&block, 2)->arg.u64 == 0);

    ASSERT_OK(kefir_irblock_appendu64(&block, KEFIR_IROPCODE_BRANCH, 0));
    ASSERT_OK(kefir_irblock_appendu64(&block, KEFIR_IROPCODE_JMP, 0));
    ASSERT_OK(kefir_ast_translator_flow_control_point_reference(&kft_mem, point1, &block, 3));
    ASSERT_OK(kefir_ast_translator_flow_control_point_reference(&kft_mem, point1, &block, 4));

    ASSERT(kefir_irblock_at(&block, 0)->opcode == KEFIR_IROPCODE_JMP);
    ASSERT(kefir_irblock_at(&block, 0)->arg.u64 == 2);
    ASSERT(kefir_irblock_at(&block, 1)->opcode == KEFIR_IROPCODE_BRANCH);
    ASSERT(kefir_irblock_at(&block, 1)->arg.u64 == 2);
    ASSERT(kefir_irblock_at(&block, 2)->opcode == KEFIR_IROPCODE_NOP);
    ASSERT(kefir_irblock_at(&block, 2)->arg.u64 == 0);
    ASSERT(kefir_irblock_at(&block, 3)->opcode == KEFIR_IROPCODE_BRANCH);
    ASSERT(kefir_irblock_at(&block, 3)->arg.u64 == 2);
    ASSERT(kefir_irblock_at(&block, 4)->opcode == KEFIR_IROPCODE_JMP);
    ASSERT(kefir_irblock_at(&block, 4)->arg.u64 == 2);

    ASSERT_OK(kefir_irblock_appendu64(&block, KEFIR_IROPCODE_NOP, 0));
    ASSERT_NOK(kefir_ast_translator_flow_control_point_reference(&kft_mem, point1, &block, 5));

    ASSERT_OK(kefir_irblock_free(&kft_mem, &block));
    ASSERT_OK(kefir_ast_flow_control_point_free(&kft_mem, point1));
}
END_CASE
