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

#ifndef KEFIR_AST_ANALYZER_TYPE_LAYOUT_H_
#define KEFIR_AST_ANALYZER_TYPE_LAYOUT_H_

#include "kefir/core/hashtree.h"
#include "kefir/ast/type.h"
#include "kefir/core/list.h"

typedef struct kefir_ast_type_layout kefir_ast_type_layout_t;

typedef struct kefir_ast_type_layout_structure_member {
    const char *identifier;
    struct kefir_ast_type_layout *layout;
} kefir_ast_type_layout_structure_member_t;

typedef struct kefir_ast_struct_type_layout {
    struct kefir_hashtree named_members;
    struct kefir_list member_list;
} kefir_ast_struct_type_layout_t;

typedef struct kefir_ast_array_type_layout {
    struct kefir_ast_type_layout *element_type;
} kefir_ast_array_type_layout_t;

typedef struct kefir_ast_custom_type_layout {
    struct kefir_list sublayouts;
} kefir_ast_custom_type_layout_t;

typedef struct kefir_ast_type_layout {
    const struct kefir_ast_type_layout *parent;
    const struct kefir_ast_type *type;
    kefir_size_t alignment;
    kefir_uptr_t value;
    kefir_bool_t bitfield;

    struct {
        kefir_size_t offset;
        kefir_size_t width;
    } bitfield_props;

    struct {
        kefir_bool_t valid;
        kefir_size_t size;
        kefir_size_t alignment;
        kefir_bool_t aligned;
        kefir_size_t relative_offset;
    } properties;

    union {
        struct kefir_ast_struct_type_layout structure_layout;
        struct kefir_ast_array_type_layout array_layout;
        struct kefir_ast_custom_type_layout custom_layout;
    };
} kefir_ast_type_layout_t;

struct kefir_ast_type_layout *kefir_ast_new_type_layout(struct kefir_mem *, const struct kefir_ast_type *, kefir_size_t,
                                                        kefir_uptr_t);

kefir_result_t kefir_ast_type_layout_free(struct kefir_mem *, struct kefir_ast_type_layout *);

kefir_result_t kefir_ast_type_layout_insert_structure_member(struct kefir_mem *, struct kefir_ast_type_layout *,
                                                             const char *, struct kefir_ast_type_layout *);

kefir_result_t kefir_ast_type_layout_add_structure_anonymous_member(struct kefir_mem *, struct kefir_ast_type_layout *,
                                                                    struct kefir_ast_type_layout *);

typedef kefir_result_t (*kefir_ast_type_layout_resolver_callback_t)(struct kefir_ast_type_layout *,
                                                                    const struct kefir_ast_designator *, void *);

kefir_result_t kefir_ast_type_layout_resolve(struct kefir_ast_type_layout *, const struct kefir_ast_designator *,
                                             struct kefir_ast_type_layout **, kefir_ast_type_layout_resolver_callback_t,
                                             void *);

kefir_result_t kefir_ast_type_layout_resolve_offset(struct kefir_ast_type_layout *, const struct kefir_ast_designator *,
                                                    struct kefir_ast_type_layout **, kefir_size_t *);

#endif
