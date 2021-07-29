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

#ifndef KEFIR_AST_TYPE_ENUM_H_
#define KEFIR_AST_TYPE_ENUM_H_

#include "kefir/ast/base.h"
#include "kefir/ast/type/base.h"

typedef struct kefir_ast_enum_enumerator {
    const char *identifier;
    struct kefir_ast_constant_expression *value;
} kefir_ast_enum_enumerator_t;

typedef struct kefir_ast_enum_type {
    kefir_bool_t complete;
    const char *identifier;
    const struct kefir_ast_type *underlying_type;
    struct kefir_list enumerators;
    struct kefir_hashtree enumerator_index;
} kefir_ast_enum_type_t;

kefir_result_t kefir_ast_enumeration_get(const struct kefir_ast_enum_type *, const char *,
                                         const struct kefir_ast_constant_expression **);

const struct kefir_ast_type *kefir_ast_type_incomplete_enumeration(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                                   const char *, const struct kefir_ast_type *);

kefir_result_t kefir_ast_enumeration_type_constant(struct kefir_mem *, struct kefir_symbol_table *,
                                                   struct kefir_ast_enum_type *, const char *,
                                                   struct kefir_ast_constant_expression *);

kefir_result_t kefir_ast_enumeration_type_constant_auto(struct kefir_mem *, struct kefir_symbol_table *,
                                                        struct kefir_ast_enum_type *, const char *);

const struct kefir_ast_type *kefir_ast_enumeration_underlying_type(const struct kefir_ast_enum_type *);

const struct kefir_ast_type *kefir_ast_type_enumeration(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                        const char *, const struct kefir_ast_type *,
                                                        struct kefir_ast_enum_type **);

#endif
