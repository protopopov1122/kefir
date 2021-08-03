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

#ifndef KEFIR_AST_INITIALIZER_H_
#define KEFIR_AST_INITIALIZER_H_

#include "kefir/ast/base.h"
#include "kefir/core/list.h"
#include "kefir/ast/designator.h"

typedef struct kefir_ast_initializer_designation {
    kefir_bool_t indexed;
    union {
        const char *identifier;
        struct kefir_ast_node_base *index;
    };

    struct kefir_ast_initializer_designation *next;
} kefir_ast_initializer_designation_t;

typedef enum kefir_ast_initializer_type {
    KEFIR_AST_INITIALIZER_EXPRESSION,
    KEFIR_AST_INITIALIZER_LIST
} kefir_ast_initializer_type_t;

typedef struct kefir_ast_initializer_list {
    struct kefir_list initializers;
} kefir_ast_initializer_list_t;

typedef struct kefir_ast_initializer {
    kefir_ast_initializer_type_t type;
    union {
        struct kefir_ast_node_base *expression;
        struct kefir_ast_initializer_list list;
    };
} kefir_ast_initializer_t;

typedef struct kefir_ast_initializer_list_entry {
    struct kefir_ast_designator *designator;
    struct kefir_ast_initializer_designation *designation;
    struct kefir_ast_initializer *value;
} kefir_ast_initializer_list_entry_t;

struct kefir_ast_initializer_designation *kefir_ast_new_initializer_member_designation(
    struct kefir_mem *, struct kefir_symbol_table *, const char *, struct kefir_ast_initializer_designation *);
struct kefir_ast_initializer_designation *kefir_ast_new_initializer_index_designation(
    struct kefir_mem *, struct kefir_ast_node_base *, struct kefir_ast_initializer_designation *);
struct kefir_ast_initializer_designation *kefir_ast_initializer_designation_clone(
    struct kefir_mem *, struct kefir_ast_initializer_designation *);
kefir_result_t kefir_ast_initializer_designation_free(struct kefir_mem *, struct kefir_ast_initializer_designation *);
kefir_result_t kefir_ast_evaluate_initializer_designation(struct kefir_mem *, const struct kefir_ast_context *,
                                                          const struct kefir_ast_initializer_designation *,
                                                          struct kefir_ast_designator **);

struct kefir_ast_initializer *kefir_ast_new_expression_initializer(struct kefir_mem *, struct kefir_ast_node_base *);
struct kefir_ast_initializer *kefir_ast_new_list_initializer(struct kefir_mem *);
kefir_result_t kefir_ast_initializer_free(struct kefir_mem *, struct kefir_ast_initializer *);
struct kefir_ast_node_base *kefir_ast_initializer_head(const struct kefir_ast_initializer *);
struct kefir_ast_initializer *kefir_ast_initializer_clone(struct kefir_mem *, const struct kefir_ast_initializer *);

kefir_result_t kefir_ast_initializer_list_init(struct kefir_ast_initializer_list *);
kefir_result_t kefir_ast_initializer_list_free(struct kefir_mem *, struct kefir_ast_initializer_list *);
kefir_result_t kefir_ast_initializer_list_append(struct kefir_mem *, struct kefir_ast_initializer_list *,
                                                 struct kefir_ast_initializer_designation *,
                                                 struct kefir_ast_initializer *);
kefir_result_t kefir_ast_initializer_list_clone(struct kefir_mem *, struct kefir_ast_initializer_list *,
                                                const struct kefir_ast_initializer_list *);

#endif
