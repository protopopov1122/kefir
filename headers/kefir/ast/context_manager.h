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

#ifndef KEFIR_AST_CONTEXT_MANAGER_H_
#define KEFIR_AST_CONTEXT_MANAGER_H_

#include "kefir/ast/context.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast/local_context.h"

typedef struct kefir_ast_context_manager {
    struct kefir_ast_global_context *global;
    struct kefir_ast_local_context *local;
    struct kefir_ast_context *current;
} kefir_ast_context_manager_t;

kefir_result_t kefir_ast_context_manager_init(struct kefir_ast_global_context *, struct kefir_ast_context_manager *);

kefir_result_t kefir_ast_context_manager_attach_local(struct kefir_ast_local_context *,
                                                      struct kefir_ast_context_manager *);

kefir_result_t kefir_ast_context_manager_detach_local(struct kefir_ast_context_manager *);

#endif
