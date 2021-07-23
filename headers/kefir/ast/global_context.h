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

#ifndef KEFIR_AST_GLOBAL_CONTEXT_H_
#define KEFIR_AST_GLOBAL_CONTEXT_H_

#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"
#include "kefir/ast/scope.h"
#include "kefir/ast/context.h"

typedef struct kefir_ast_global_context {
    struct kefir_ast_context context;

    struct kefir_symbol_table symbols;
    const struct kefir_ast_type_traits *type_traits;
    struct kefir_ast_type_bundle type_bundle;
    const struct kefir_ast_target_environment *target_env;
    struct kefir_list function_decl_contexts;

    struct kefir_ast_identifier_flat_scope object_identifiers;
    struct kefir_ast_identifier_flat_scope constant_identifiers;
    struct kefir_ast_identifier_flat_scope type_identifiers;
    struct kefir_ast_identifier_flat_scope function_identifiers;

    struct kefir_ast_identifier_flat_scope ordinary_scope;
    struct kefir_ast_identifier_flat_scope tag_scope;
    struct kefir_ast_context_temporaries temporaries;
} kefir_ast_global_context_t;

kefir_result_t kefir_ast_global_context_init(struct kefir_mem *, const struct kefir_ast_type_traits *,
                                             const struct kefir_ast_target_environment *,
                                             struct kefir_ast_global_context *);
kefir_result_t kefir_ast_global_context_free(struct kefir_mem *, struct kefir_ast_global_context *);

kefir_result_t kefir_ast_global_context_resolve_scoped_ordinary_identifier(const struct kefir_ast_global_context *,
                                                                           const char *,
                                                                           const struct kefir_ast_scoped_identifier **);

kefir_result_t kefir_ast_global_context_resolve_scoped_tag_identifier(const struct kefir_ast_global_context *,
                                                                      const char *,
                                                                      const struct kefir_ast_scoped_identifier **);

kefir_result_t kefir_ast_global_context_declare_external(struct kefir_mem *, struct kefir_ast_global_context *,
                                                         const char *, const struct kefir_ast_type *,
                                                         struct kefir_ast_alignment *,
                                                         const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_declare_external_thread_local(struct kefir_mem *,
                                                                      struct kefir_ast_global_context *, const char *,
                                                                      const struct kefir_ast_type *,
                                                                      struct kefir_ast_alignment *,
                                                                      const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_define_external(struct kefir_mem *, struct kefir_ast_global_context *,
                                                        const char *, const struct kefir_ast_type *,
                                                        struct kefir_ast_alignment *, struct kefir_ast_initializer *,
                                                        const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_define_external_thread_local(
    struct kefir_mem *, struct kefir_ast_global_context *, const char *, const struct kefir_ast_type *,
    struct kefir_ast_alignment *, struct kefir_ast_initializer *, const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_define_static(struct kefir_mem *, struct kefir_ast_global_context *,
                                                      const char *, const struct kefir_ast_type *,
                                                      struct kefir_ast_alignment *, struct kefir_ast_initializer *,
                                                      const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_define_static_thread_local(
    struct kefir_mem *, struct kefir_ast_global_context *, const char *, const struct kefir_ast_type *,
    struct kefir_ast_alignment *, struct kefir_ast_initializer *, const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_define_constant(struct kefir_mem *, struct kefir_ast_global_context *,
                                                        const char *, struct kefir_ast_constant_expression *,
                                                        const struct kefir_ast_type *,
                                                        const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_define_tag(struct kefir_mem *, struct kefir_ast_global_context *,
                                                   const struct kefir_ast_type *,
                                                   const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_define_type(struct kefir_mem *, struct kefir_ast_global_context *, const char *,
                                                    const struct kefir_ast_type *,
                                                    const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_declare_function(struct kefir_mem *, struct kefir_ast_global_context *,
                                                         kefir_ast_function_specifier_t, const struct kefir_ast_type *,
                                                         const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_define_function(struct kefir_mem *, struct kefir_ast_global_context *,
                                                        kefir_ast_function_specifier_t, const struct kefir_ast_type *,
                                                        const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_global_context_define_static_function(struct kefir_mem *, struct kefir_ast_global_context *,
                                                               kefir_ast_function_specifier_t,
                                                               const struct kefir_ast_type *,
                                                               const struct kefir_ast_scoped_identifier **);

#endif
