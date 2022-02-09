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

#ifndef KEFIR_AST_TRANSLATOR_FUNCTION_DECLARATION_H_
#define KEFIR_AST_TRANSLATOR_FUNCTION_DECLARATION_H_

#include "kefir/ast/type.h"
#include "kefir/ir/module.h"
#include "kefir/ast/type_layout.h"
#include "kefir/core/list.h"
#include "kefir/core/hashtree.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ast-translator/base.h"

typedef struct kefir_ast_translator_function_declaration {
    const struct kefir_ast_type *function_type;
    struct kefir_ir_function_decl *ir_function_decl;
    kefir_id_t ir_argument_type_id;
    struct kefir_ir_type *ir_argument_type;
    kefir_id_t ir_return_type_id;
    struct kefir_ir_type *ir_return_type;

    struct kefir_list argument_layouts;
    struct kefir_ast_type_layout *return_layout;
} kefir_ast_translator_function_declaration_t;

kefir_result_t kefir_ast_translator_function_declaration_init(
    struct kefir_mem *, const struct kefir_ast_translator_environment *, struct kefir_ast_type_bundle *,
    const struct kefir_ast_type_traits *, struct kefir_ir_module *, const char *, const struct kefir_ast_type *,
    const struct kefir_list *, struct kefir_ast_translator_function_declaration **);

kefir_result_t kefir_ast_translator_function_declaration_free(struct kefir_mem *,
                                                              struct kefir_ast_translator_function_declaration *);

#endif
