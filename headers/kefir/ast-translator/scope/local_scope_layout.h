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

#ifndef KEFIR_AST_TRANSLATOR_SCOPE_LOCAL_SCOPE_LAYOUT_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_LOCAL_SCOPE_LAYOUT_H_

#include "kefir/ast-translator/scope/scoped_identifier.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/core/list.h"

typedef struct kefir_ast_translator_local_scope_layout {
    const struct kefir_ast_local_context *local_context;
    struct kefir_ast_translator_global_scope_layout *global;
    struct kefir_ir_type *local_layout;
    kefir_id_t local_layout_id;
    struct kefir_ast_type_layout *local_type_layout;

    struct kefir_list local_objects;
    struct kefir_list static_objects;
    struct kefir_list static_thread_local_objects;
} kefir_ast_translator_local_scope_layout_t;

kefir_result_t kefir_ast_translator_local_scope_layout_init(struct kefir_mem *, struct kefir_ir_module *,
                                                            struct kefir_ast_translator_global_scope_layout *,
                                                            struct kefir_ast_translator_local_scope_layout *);

kefir_result_t kefir_ast_translator_local_scope_layout_free(struct kefir_mem *,
                                                            struct kefir_ast_translator_local_scope_layout *);

kefir_result_t kefir_ast_translator_build_local_scope_layout(struct kefir_mem *, const struct kefir_ast_local_context *,
                                                             const struct kefir_ast_translator_environment *,
                                                             struct kefir_ir_module *,
                                                             struct kefir_ast_translator_local_scope_layout *);

#endif
