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

#ifndef KEFIR_AST_CONTEXT_H_
#define KEFIR_AST_CONTEXT_H_

#include "kefir/core/mem.h"
#include "kefir/ast/base.h"
#include "kefir/ast/scope.h"
#include "kefir/ast/target_environment.h"
#include "kefir/ast/temporaries.h"
#include "kefir/ast/flow_control.h"

typedef struct kefir_ast_context {
    kefir_result_t (*resolve_ordinary_identifier)(const struct kefir_ast_context *, const char *,
                                                  const struct kefir_ast_scoped_identifier **);
    kefir_result_t (*resolve_tag_identifier)(const struct kefir_ast_context *, const char *,
                                             const struct kefir_ast_scoped_identifier **);
    kefir_result_t (*resolve_label_identifier)(const struct kefir_ast_context *, const char *,
                                               const struct kefir_ast_scoped_identifier **);
    kefir_result_t (*allocate_temporary_value)(struct kefir_mem *, const struct kefir_ast_context *,
                                               const struct kefir_ast_type *, const struct kefir_source_location *,
                                               struct kefir_ast_temporary_identifier *);

    kefir_result_t (*define_tag)(struct kefir_mem *, const struct kefir_ast_context *, const struct kefir_ast_type *,
                                 const struct kefir_source_location *);
    kefir_result_t (*define_constant)(struct kefir_mem *, const struct kefir_ast_context *, const char *,
                                      struct kefir_ast_constant_expression *, const struct kefir_ast_type *,
                                      const struct kefir_source_location *);
    kefir_result_t (*define_identifier)(struct kefir_mem *, const struct kefir_ast_context *, kefir_bool_t,
                                        const char *, const struct kefir_ast_type *,
                                        kefir_ast_scoped_identifier_storage_t, kefir_ast_function_specifier_t,
                                        struct kefir_ast_alignment *, struct kefir_ast_initializer *,
                                        const struct kefir_source_location *,
                                        const struct kefir_ast_scoped_identifier **);
    kefir_result_t (*reference_label)(struct kefir_mem *, const struct kefir_ast_context *, const char *, kefir_bool_t,
                                      const struct kefir_source_location *,
                                      const struct kefir_ast_scoped_identifier **);
    kefir_result_t (*push_block)(struct kefir_mem *, const struct kefir_ast_context *);
    kefir_result_t (*pop_block)(struct kefir_mem *, const struct kefir_ast_context *);

    struct kefir_symbol_table *symbols;
    const struct kefir_ast_type_traits *type_traits;
    struct kefir_ast_type_bundle *type_bundle;
    const struct kefir_ast_target_environment *target_env;
    struct kefir_ast_context_temporaries *temporaries;
    kefir_ast_type_analysis_context_t type_analysis_context;
    struct kefir_ast_flow_control_tree *flow_control_tree;
    struct kefir_ast_global_context *global_context;
    struct kefir_list *function_decl_contexts;
    const struct kefir_ast_scoped_identifier *surrounding_function;

    void *payload;
} kefir_ast_context_t;

#endif
