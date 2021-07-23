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

#ifndef KEFIR_AST_DESGINATOR_H_
#define KEFIR_AST_DESGINATOR_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"

typedef enum kefir_ast_designator_type {
    KEFIR_AST_DESIGNATOR_MEMBER,
    KEFIR_AST_DESIGNATOR_SUBSCRIPT
} kefir_ast_designator_type_t;

typedef struct kefir_ast_designator {
    kefir_ast_designator_type_t type;
    union {
        const char *member;
        kefir_size_t index;
    };
    struct kefir_ast_designator *next;
} kefir_ast_designator_t;

struct kefir_ast_designator *kefir_ast_new_member_designator(struct kefir_mem *, struct kefir_symbol_table *,
                                                             const char *, struct kefir_ast_designator *);

struct kefir_ast_designator *kefir_ast_new_index_designator(struct kefir_mem *, kefir_size_t,
                                                            struct kefir_ast_designator *);

kefir_result_t kefir_ast_designator_free(struct kefir_mem *, struct kefir_ast_designator *);

struct kefir_ast_designator *kefir_ast_designator_clone(struct kefir_mem *, const struct kefir_ast_designator *);

#endif
