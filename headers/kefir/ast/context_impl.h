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

#ifndef KEFIR_AST_CONTEXT_IMPL_H_
#define KEFIR_AST_CONTEXT_IMPL_H_

#include "kefir/core/mem.h"
#include "kefir/ast/scope.h"

kefir_result_t kefir_ast_context_free_scoped_identifier(struct kefir_mem *, struct kefir_ast_scoped_identifier *,
                                                        void *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_object_identifier(
    struct kefir_mem *, const struct kefir_ast_type *, kefir_ast_scoped_identifier_storage_t,
    struct kefir_ast_alignment *, kefir_ast_scoped_identifier_linkage_t, kefir_bool_t, struct kefir_ast_initializer *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_constant(struct kefir_mem *,
                                                                               struct kefir_ast_constant_expression *,
                                                                               const struct kefir_ast_type *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_type_tag(struct kefir_mem *,
                                                                               const struct kefir_ast_type *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_type_definition(struct kefir_mem *,
                                                                                      const struct kefir_ast_type *);

kefir_result_t kefir_ast_context_type_retrieve_tag(const struct kefir_ast_type *, const char **);

kefir_result_t kefir_ast_context_update_existing_scoped_type_tag(struct kefir_ast_scoped_identifier *,
                                                                 const struct kefir_ast_type *);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_function_identifier(
    struct kefir_mem *, const struct kefir_ast_type *, kefir_ast_function_specifier_t,
    kefir_ast_scoped_identifier_storage_t, kefir_bool_t);

kefir_ast_function_specifier_t kefir_ast_context_merge_function_specifiers(kefir_ast_function_specifier_t,
                                                                           kefir_ast_function_specifier_t);

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_label(struct kefir_mem *, kefir_bool_t);

kefir_result_t kefir_ast_context_merge_alignment(struct kefir_mem *, struct kefir_ast_alignment **,
                                                 struct kefir_ast_alignment *);

#endif
