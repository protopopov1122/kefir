/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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

#ifndef KEFIR_AST_TRANSLATOR_SCOPE_TRANSLATOR_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_TRANSLATOR_H_

#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast/context.h"
#include "kefir/ir/module.h"

kefir_result_t kefir_ast_translate_global_scope(struct kefir_mem *, const struct kefir_ast_context *,
                                                struct kefir_ir_module *,
                                                const struct kefir_ast_translator_global_scope_layout *);

kefir_result_t kefir_ast_translate_data_initializer(struct kefir_mem *, const struct kefir_ast_context *,
                                                    struct kefir_ir_module *, struct kefir_ast_type_layout *,
                                                    const struct kefir_ir_type *, const struct kefir_ast_initializer *,
                                                    struct kefir_ir_data *, kefir_size_t);

#endif
