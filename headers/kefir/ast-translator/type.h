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

#ifndef KEFIR_AST_TRANSLATOR_TYPE_H_
#define KEFIR_AST_TRANSLATOR_TYPE_H_

#include "kefir/ir/type.h"
#include "kefir/ast/type.h"
#include "kefir/ast/type_layout.h"
#include "kefir/ast-translator/function_declaration.h"

typedef struct kefir_ast_translator_type {
    const struct kefir_ast_type *type;
    struct {
        kefir_id_t ir_type_id;
        struct kefir_ir_type *ir_type;
        struct kefir_ast_type_layout *layout;
    } object;
} kefir_ast_translator_type_t;

kefir_result_t kefir_ast_translator_type_new(struct kefir_mem *, const struct kefir_ast_translator_environment *,
                                             struct kefir_ir_module *, const struct kefir_ast_type *, kefir_size_t,
                                             struct kefir_ast_translator_type **);

kefir_result_t kefir_ast_translator_type_free(struct kefir_mem *, struct kefir_ast_translator_type *);

#endif
