/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KEFIR_AST_TRANSLATOR_FLOW_CONTROL_H_
#define KEFIR_AST_TRANSLATOR_FLOW_CONTROL_H_

#include "kefir/core/list.h"
#include "kefir/ast/flow_control.h"
#include "kefir/ir/module.h"

typedef struct kefir_ast_translator_flow_control_point {
    kefir_bool_t resolved;
    union {
        kefir_uint64_t address;
        struct kefir_list dependents;
    };
} kefir_ast_translator_flow_control_point_t;

kefir_result_t kefir_ast_translator_flow_control_point_init(struct kefir_mem *, struct kefir_ast_flow_control_point *,
                                                            struct kefir_ast_translator_flow_control_point **);

kefir_result_t kefir_ast_translator_flow_control_point_reference(struct kefir_mem *,
                                                                 struct kefir_ast_flow_control_point *,
                                                                 struct kefir_irblock *, kefir_size_t);

kefir_result_t kefir_ast_translator_flow_control_point_resolve(struct kefir_mem *,
                                                               struct kefir_ast_flow_control_point *, kefir_uint64_t);

kefir_result_t kefir_ast_translator_flow_control_tree_init(struct kefir_mem *, struct kefir_ast_flow_control_tree *);

#endif
