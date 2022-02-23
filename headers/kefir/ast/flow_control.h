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

#ifndef KEFIR_AST_FLOW_CONTROL_H_
#define KEFIR_AST_FLOW_CONTROL_H_

#include "kefir/ast/base.h"
#include "kefir/core/tree.h"
#include "kefir/core/hashtree.h"
#include "kefir/ast/type.h"

typedef struct kefir_ast_flow_control_structure kefir_ast_flow_control_structure_t;
typedef struct kefir_ast_flow_control_point kefir_ast_flow_control_point_t;

typedef struct kefir_ast_flow_control_structure_data_element_range {
    struct kefir_list_entry *head;
    struct kefir_list_entry *tail;
} kefir_ast_flow_control_structure_data_element_range_t;

#define KEFIR_AST_FLOW_CONTROL_PAYLOAD_SIZE (sizeof(kefir_uptr_t) * 4)

typedef struct kefir_ast_flow_control_point_cleanup {
    kefir_result_t (*callback)(struct kefir_mem *, struct kefir_ast_flow_control_point *, void *);
    void *payload;
} kefir_ast_flow_control_point_cleanup_t;

typedef struct kefir_ast_flow_control_point {
    struct kefir_ast_flow_control_structure *parent;
    struct kefir_ast_flow_control_structure_data_element_range parent_data_elts;
    unsigned char content[KEFIR_AST_FLOW_CONTROL_PAYLOAD_SIZE];
    void *ptr;
    struct kefir_ast_flow_control_point_cleanup cleanup;
} kefir_ast_flow_control_point_t;

struct kefir_ast_flow_control_point *kefir_ast_flow_control_point_alloc(struct kefir_mem *,
                                                                        struct kefir_ast_flow_control_structure *);
kefir_result_t kefir_ast_flow_control_point_free(struct kefir_mem *, struct kefir_ast_flow_control_point *);

typedef enum kefir_ast_flow_control_data_element_type {
    KEFIR_AST_FLOW_CONTROL_DATA_ELEMENT_VLA
} kefir_ast_flow_control_data_element_type_t;

typedef struct kefir_ast_flow_control_data_element {
    kefir_id_t identifier;
    struct kefir_ast_flow_control_structure *parent;
    kefir_ast_flow_control_data_element_type_t type;
} kefir_ast_flow_control_data_element_t;

struct kefir_ast_flow_control_data_element *kefir_ast_flow_control_data_element_alloc(
    struct kefir_mem *, kefir_id_t, struct kefir_ast_flow_control_structure *,
    kefir_ast_flow_control_data_element_type_t);
kefir_result_t kefir_ast_flow_control_data_element_free(struct kefir_mem *,
                                                        struct kefir_ast_flow_control_data_element *);

typedef struct kefir_ast_flow_control_structure_data_elements {
    struct kefir_list elements;
} kefir_ast_flow_control_structure_data_elements_t;

kefir_result_t kefir_ast_flow_control_structure_data_elements_init(
    struct kefir_ast_flow_control_structure_data_elements *);
kefir_result_t kefir_ast_flow_control_structure_data_elements_free(
    struct kefir_mem *, struct kefir_ast_flow_control_structure_data_elements *);
kefir_result_t kefir_ast_flow_control_structure_data_elements_append(
    struct kefir_mem *, struct kefir_ast_flow_control_structure_data_elements *,
    struct kefir_ast_flow_control_data_element *);
kefir_result_t kefir_ast_flow_control_structure_data_element_head(
    const struct kefir_ast_flow_control_structure_data_elements *, const struct kefir_ast_flow_control_data_element **);

kefir_result_t kefir_ast_flow_control_structure_data_elements_current_range(
    const struct kefir_ast_flow_control_structure_data_elements *,
    struct kefir_ast_flow_control_structure_data_element_range *);

typedef enum kefir_ast_flow_control_structure_type {
    KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK,
    KEFIR_AST_FLOW_CONTROL_STRUCTURE_IF,
    KEFIR_AST_FLOW_CONTROL_STRUCTURE_SWITCH,
    KEFIR_AST_FLOW_CONTROL_STRUCTURE_FOR,
    KEFIR_AST_FLOW_CONTROL_STRUCTURE_WHILE,
    KEFIR_AST_FLOW_CONTROL_STRUCTURE_DO
} kefir_ast_flow_control_structure_type_t;

typedef struct kefir_ast_flow_control_structure_cleanup {
    kefir_result_t (*callback)(struct kefir_mem *, struct kefir_ast_flow_control_structure *, void *);
    void *payload;
} kefir_ast_flow_control_structure_cleanup_t;

typedef struct kefir_ast_flow_control_structure {
    struct kefir_ast_flow_control_point *parent_point;
    kefir_ast_flow_control_structure_type_t type;
    struct kefir_ast_flow_control_structure_cleanup cleanup;
    struct {
        unsigned char content[KEFIR_AST_FLOW_CONTROL_PAYLOAD_SIZE];
        void *ptr;
    } payload;

    union {
        struct {
            kefir_bool_t contains_vla;
            struct kefir_ast_flow_control_structure_data_elements data_elements;
        } block;

        struct {
            struct kefir_ast_flow_control_point *thenBranchEnd;
            struct kefir_ast_flow_control_point *elseBranchEnd;
        } conditional;

        struct {
            struct kefir_hashtree cases;
            struct kefir_ast_flow_control_point *defaultCase;
            const struct kefir_ast_type *controlling_expression_type;
            struct kefir_ast_flow_control_point *end;
        } switchStatement;

        struct {
            struct kefir_ast_flow_control_point *continuation;
            struct kefir_ast_flow_control_point *end;
        } loop;
    } value;
} kefir_ast_flow_control_structure_t;

typedef struct kefir_ast_flow_control_tree {
    struct kefir_tree_node root;
    struct kefir_tree_node *current;

    struct {
        kefir_id_t vla_id;
    } data_element_track;
} kefir_ast_flow_control_tree_t;

#define KEFIR_AST_FLOW_CONTROL_SET_CLEANUP(_flow_control, _callback, _payload) \
    do {                                                                       \
        (_flow_control)->cleanup.callback = (_callback);                       \
        (_flow_control)->cleanup.payload = (_payload);                         \
    } while (0)

kefir_result_t kefir_ast_flow_control_tree_init(struct kefir_ast_flow_control_tree *);
kefir_result_t kefir_ast_flow_control_tree_free(struct kefir_mem *, struct kefir_ast_flow_control_tree *);
kefir_result_t kefir_ast_flow_control_tree_push(struct kefir_mem *, struct kefir_ast_flow_control_tree *,
                                                kefir_ast_flow_control_structure_type_t,
                                                struct kefir_ast_flow_control_structure **);
kefir_result_t kefir_ast_flow_control_tree_pop(struct kefir_ast_flow_control_tree *);
kefir_result_t kefir_ast_flow_control_tree_top(struct kefir_ast_flow_control_tree *,
                                               struct kefir_ast_flow_control_structure **);
kefir_result_t kefir_ast_flow_control_tree_traverse(struct kefir_ast_flow_control_tree *,
                                                    kefir_result_t (*)(const struct kefir_ast_flow_control_structure *,
                                                                       void *, kefir_bool_t *),
                                                    void *, struct kefir_ast_flow_control_structure **);

kefir_result_t kefir_ast_flow_control_block_add_data_element(struct kefir_mem *,
                                                             struct kefir_ast_flow_control_structure *,
                                                             struct kefir_ast_flow_control_data_element *);

kefir_result_t kefir_ast_flow_control_point_common_parent(struct kefir_ast_flow_control_point *,
                                                          struct kefir_ast_flow_control_point *,
                                                          struct kefir_ast_flow_control_structure **);

kefir_result_t kefir_ast_flow_control_point_parents(struct kefir_mem *, struct kefir_ast_flow_control_point *,
                                                    struct kefir_list *, struct kefir_ast_flow_control_structure *);

#endif
