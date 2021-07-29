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

#ifndef KEFIR_AST_TYPE_STRUCT_H_
#define KEFIR_AST_TYPE_STRUCT_H_

#include "kefir/ast/type/base.h"
#include "kefir/ast/alignment.h"

typedef struct kefir_ast_struct_field {
    const char *identifier;
    const struct kefir_ast_type *type;
    struct kefir_ast_alignment *alignment;
    kefir_bool_t bitfield;
    struct kefir_ast_constant_expression *bitwidth;
} kefir_ast_struct_field_t;

typedef struct kefir_ast_struct_type {
    kefir_bool_t complete;
    const char *identifier;
    struct kefir_list fields;
    struct kefir_hashtree field_index;
} kefir_ast_struct_type_t;

const struct kefir_ast_type *kefir_ast_type_incomplete_structure(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                                 const char *);
const struct kefir_ast_type *kefir_ast_type_incomplete_union(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                             const char *);

kefir_result_t kefir_ast_struct_type_get_field(const struct kefir_ast_struct_type *, const char *,
                                               const struct kefir_ast_struct_field **);

kefir_result_t kefir_ast_struct_type_resolve_field(const struct kefir_ast_struct_type *, const char *,
                                                   const struct kefir_ast_struct_field **);

kefir_result_t kefir_ast_struct_type_field(struct kefir_mem *, struct kefir_symbol_table *,
                                           struct kefir_ast_struct_type *, const char *, const struct kefir_ast_type *,
                                           struct kefir_ast_alignment *);

kefir_result_t kefir_ast_struct_type_bitfield(struct kefir_mem *, struct kefir_symbol_table *,
                                              struct kefir_ast_struct_type *, const char *,
                                              const struct kefir_ast_type *, struct kefir_ast_alignment *,
                                              struct kefir_ast_constant_expression *);

const struct kefir_ast_type *kefir_ast_type_structure(struct kefir_mem *, struct kefir_ast_type_bundle *, const char *,
                                                      struct kefir_ast_struct_type **);
const struct kefir_ast_type *kefir_ast_type_union(struct kefir_mem *, struct kefir_ast_type_bundle *, const char *,
                                                  struct kefir_ast_struct_type **);

#endif
