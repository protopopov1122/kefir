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

#include "kefir/ast/flow_control.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>

struct kefir_ast_flow_control_point *kefir_ast_flow_control_point_alloc(
    struct kefir_mem *mem, struct kefir_ast_flow_control_structure *parent) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_flow_control_point *point = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_flow_control_point));
    REQUIRE(point != NULL, NULL);
    point->ptr = &point->content[0];
    memset(point->content, 0, KEFIR_AST_FLOW_CONTROL_PAYLOAD_SIZE);
    point->cleanup.callback = NULL;
    point->cleanup.payload = NULL;
    point->parent = parent;

    if (parent != NULL && parent->type == KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK) {
        REQUIRE_ELSE(kefir_ast_flow_control_structure_data_elements_current_range(&parent->value.block.data_elements,
                                                                                  &point->parent_data_elts) == KEFIR_OK,
                     {
                         KEFIR_FREE(mem, point);
                         return NULL;
                     });
    } else {
        point->parent_data_elts.head = NULL;
        point->parent_data_elts.tail = NULL;
    }
    return point;
}

kefir_result_t kefir_ast_flow_control_point_free(struct kefir_mem *mem, struct kefir_ast_flow_control_point *point) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(point != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control point"));

    if (point->cleanup.callback != NULL) {
        REQUIRE_OK(point->cleanup.callback(mem, point, point->cleanup.payload));
        point->cleanup.callback = NULL;
        point->cleanup.payload = NULL;
    }
    point->ptr = NULL;
    KEFIR_FREE(mem, point);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_point_bound(struct kefir_ast_flow_control_point *point) {
    REQUIRE(point != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control point"));

    if (point->parent != NULL && point->parent->type == KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK) {
        REQUIRE_OK(kefir_ast_flow_control_structure_data_elements_current_range(
            &point->parent->value.block.data_elements, &point->parent_data_elts));
    } else {
        point->parent_data_elts.head = NULL;
        point->parent_data_elts.tail = NULL;
    }
    return KEFIR_OK;
}

struct kefir_ast_flow_control_data_element *kefir_ast_flow_control_data_element_alloc(
    struct kefir_mem *mem, kefir_id_t identifier, struct kefir_ast_flow_control_structure *parent,
    kefir_ast_flow_control_data_element_type_t type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(parent != NULL, NULL);

    struct kefir_ast_flow_control_data_element *element =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_flow_control_data_element));
    REQUIRE(element != NULL, NULL);
    element->identifier = identifier;
    element->parent = parent;
    element->type = type;
    return element;
}

kefir_result_t kefir_ast_flow_control_data_element_free(struct kefir_mem *mem,
                                                        struct kefir_ast_flow_control_data_element *element) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(element != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control data element"));

    KEFIR_FREE(mem, element);
    return KEFIR_OK;
}

static kefir_result_t data_element_free(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                        void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    REQUIRE_OK(kefir_ast_flow_control_data_element_free(mem, entry->value));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_structure_data_elements_init(
    struct kefir_ast_flow_control_structure_data_elements *elts) {
    REQUIRE(elts != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                                          "Expected valid pointer to AST flow control structure data elements"));

    REQUIRE_OK(kefir_list_init(&elts->elements));
    REQUIRE_OK(kefir_list_on_remove(&elts->elements, data_element_free, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_structure_data_elements_free(
    struct kefir_mem *mem, struct kefir_ast_flow_control_structure_data_elements *elts) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(elts != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                                          "Expected valid pointer to AST flow control structure data elements"));

    REQUIRE_OK(kefir_list_free(mem, &elts->elements));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_structure_data_elements_append(
    struct kefir_mem *mem, struct kefir_ast_flow_control_structure_data_elements *elts,
    struct kefir_ast_flow_control_data_element *element) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(elts != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                                          "Expected valid pointer to AST flow control structure data elements"));
    REQUIRE(element != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST flow control data element"));

    REQUIRE_OK(kefir_list_insert_after(mem, &elts->elements, kefir_list_tail(&elts->elements), element));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_structure_data_element_head(
    const struct kefir_ast_flow_control_structure_data_elements *elts,
    const struct kefir_ast_flow_control_data_element **element) {
    REQUIRE(elts != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                                          "Expected valid pointer to AST flow control structure data elements"));
    REQUIRE(element != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST flow control data element"));

    struct kefir_list_entry *head = kefir_list_head(&elts->elements);
    REQUIRE(head != NULL,
            KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "AST flow control structure data element list is empty"));
    *element = head->value;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_structure_data_elements_current_range(
    const struct kefir_ast_flow_control_structure_data_elements *elts,
    struct kefir_ast_flow_control_structure_data_element_range *range) {
    REQUIRE(elts != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                                          "Expected valid pointer to AST flow control structure data elements"));
    REQUIRE(range != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER,
                                           "Expected valid pointer to AST flow control structure data element range"));

    range->head = kefir_list_head(&elts->elements);
    range->tail = kefir_list_tail(&elts->elements);
    return KEFIR_OK;
}

static kefir_result_t flow_control_statement_free(struct kefir_mem *mem, void *node, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_OK);

    ASSIGN_DECL_CAST(struct kefir_ast_flow_control_structure *, statement, node);
    if (statement->cleanup.callback != NULL) {
        statement->cleanup.callback(mem, statement, statement->cleanup.payload);
        statement->cleanup.callback = NULL;
        statement->cleanup.payload = NULL;
    }
    statement->payload.ptr = NULL;

    if (statement->parent_point != NULL) {
        REQUIRE_OK(kefir_ast_flow_control_point_free(mem, statement->parent_point));
        statement->parent_point = NULL;
    }

    switch (statement->type) {
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK:
            REQUIRE_OK(kefir_ast_flow_control_structure_data_elements_free(mem, &statement->value.block.data_elements));
            break;

        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_IF:
            if (statement->value.conditional.thenBranchEnd != NULL) {
                REQUIRE_OK(kefir_ast_flow_control_point_free(mem, statement->value.conditional.thenBranchEnd));
                statement->value.conditional.thenBranchEnd = NULL;
            }
            if (statement->value.conditional.elseBranchEnd != NULL) {
                REQUIRE_OK(kefir_ast_flow_control_point_free(mem, statement->value.conditional.elseBranchEnd));
                statement->value.conditional.elseBranchEnd = NULL;
            }
            break;

        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_SWITCH:
            REQUIRE_OK(kefir_hashtree_free(mem, &statement->value.switchStatement.cases));
            if (statement->value.switchStatement.defaultCase != NULL) {
                REQUIRE_OK(kefir_ast_flow_control_point_free(mem, statement->value.switchStatement.defaultCase));
                statement->value.switchStatement.defaultCase = NULL;
            }

            if (statement->value.switchStatement.end != NULL) {
                REQUIRE_OK(kefir_ast_flow_control_point_free(mem, statement->value.switchStatement.end));
                statement->value.switchStatement.end = NULL;
            }
            break;

        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_FOR:
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_WHILE:
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_DO:
            if (statement->value.loop.continuation != NULL) {
                REQUIRE_OK(kefir_ast_flow_control_point_free(mem, statement->value.loop.continuation));
                statement->value.loop.continuation = NULL;
            }

            if (statement->value.loop.end != NULL) {
                REQUIRE_OK(kefir_ast_flow_control_point_free(mem, statement->value.loop.end));
                statement->value.loop.end = NULL;
            }
            break;
    }
    KEFIR_FREE(mem, statement);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_init(struct kefir_ast_flow_control_tree *tree) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control tree"));
    REQUIRE_OK(kefir_tree_init(&tree->root, NULL));
    REQUIRE_OK(kefir_tree_on_removal(&tree->root, flow_control_statement_free, NULL));
    tree->current = &tree->root;
    tree->data_element_track.vla_id = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_free(struct kefir_mem *mem, struct kefir_ast_flow_control_tree *tree) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control tree"));

    REQUIRE_OK(kefir_tree_free(mem, &tree->root));
    tree->current = NULL;
    tree->data_element_track.vla_id = 0;
    return KEFIR_OK;
}

static kefir_result_t point_tree_free(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key,
                                      kefir_hashtree_value_t value, void *payload) {
    UNUSED(key);
    UNUSED(tree);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(struct kefir_ast_flow_control_point *, point, value);
    REQUIRE(point != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control point"));
    REQUIRE_OK(kefir_ast_flow_control_point_free(mem, point));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_push(struct kefir_mem *mem, struct kefir_ast_flow_control_tree *tree,
                                                kefir_ast_flow_control_structure_type_t type,
                                                struct kefir_ast_flow_control_structure **statement) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control tree"));

    struct kefir_ast_flow_control_structure *parent = NULL;
    REQUIRE_OK(kefir_ast_flow_control_tree_top(tree, &parent));
    struct kefir_ast_flow_control_structure *stmt = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_flow_control_structure));
    REQUIRE(stmt != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST flow control statement"));
    stmt->type = type;
    stmt->cleanup.callback = NULL;
    stmt->cleanup.payload = NULL;
    stmt->payload.ptr = &stmt->payload.content[0];
    memset(&stmt->payload.content[0], 0, KEFIR_AST_FLOW_CONTROL_PAYLOAD_SIZE);

    if (parent != NULL) {
        stmt->parent_point = kefir_ast_flow_control_point_alloc(mem, parent);
        REQUIRE_ELSE(stmt->parent_point != NULL, {
            KEFIR_FREE(mem, stmt);
            return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST flow control point");
        });
    } else {
        stmt->parent_point = NULL;
    }

    switch (type) {
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK:
            stmt->value.block.contains_vla = false;
            REQUIRE_OK(kefir_ast_flow_control_structure_data_elements_init(&stmt->value.block.data_elements));
            break;

        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_IF:
            stmt->value.conditional.thenBranchEnd = NULL;
            stmt->value.conditional.elseBranchEnd = NULL;
            break;

        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_SWITCH: {
            kefir_result_t res = kefir_hashtree_init(&stmt->value.switchStatement.cases, &kefir_hashtree_uint_ops);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_FREE(mem, stmt);
                return res;
            });

            res = kefir_hashtree_on_removal(&stmt->value.switchStatement.cases, point_tree_free, NULL);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_hashtree_free(mem, &stmt->value.switchStatement.cases);
                KEFIR_FREE(mem, stmt);
                return res;
            });

            stmt->value.switchStatement.defaultCase = NULL;
            stmt->value.switchStatement.controlling_expression_type = NULL;
            stmt->value.switchStatement.end = NULL;
        } break;

        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_FOR:
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_WHILE:
        case KEFIR_AST_FLOW_CONTROL_STRUCTURE_DO:
            stmt->value.loop.continuation = NULL;
            stmt->value.loop.end = NULL;
            break;
    }

    kefir_result_t res = kefir_tree_insert_child(mem, tree->current, stmt, &tree->current);
    REQUIRE_ELSE(res == KEFIR_OK, {
        if (type == KEFIR_AST_FLOW_CONTROL_STRUCTURE_SWITCH) {
            kefir_hashtree_free(mem, &stmt->value.switchStatement.cases);
        }
        KEFIR_FREE(mem, stmt);
        return res;
    });
    ASSIGN_PTR(statement, stmt);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_pop(struct kefir_ast_flow_control_tree *tree) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control tree"));
    REQUIRE(tree->current->parent != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot pop flow control tree top-level statement"));

    tree->current = tree->current->parent;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_top(struct kefir_ast_flow_control_tree *tree,
                                               struct kefir_ast_flow_control_structure **stmt) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control tree"));
    REQUIRE(stmt != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST flow control tree statement"));

    ASSIGN_DECL_CAST(struct kefir_ast_flow_control_structure *, statement, tree->current->value);
    ASSIGN_PTR(stmt, statement);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_traverse(
    struct kefir_ast_flow_control_tree *tree,
    kefir_result_t (*callback)(const struct kefir_ast_flow_control_structure *, void *, kefir_bool_t *), void *payload,
    struct kefir_ast_flow_control_structure **stmt) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control tree"));
    REQUIRE(callback != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid traversal callback"));
    REQUIRE(stmt != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST flow control tree statement"));

    struct kefir_ast_flow_control_structure *current = NULL;
    REQUIRE_OK(kefir_ast_flow_control_tree_top(tree, &current));

    while (current != NULL) {
        kefir_bool_t match = false;
        REQUIRE_OK(callback(current, payload, &match));
        if (match) {
            ASSIGN_PTR(stmt, current);
            return KEFIR_OK;
        } else {
            current = current->parent_point != NULL ? current->parent_point->parent : NULL;
        }
    }

    return KEFIR_NOT_FOUND;
}

kefir_result_t kefir_ast_flow_control_block_add_data_element(struct kefir_mem *mem,
                                                             struct kefir_ast_flow_control_structure *stmt,
                                                             struct kefir_ast_flow_control_data_element *element) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(stmt != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control tree statement"));
    REQUIRE(element != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control data element"));
    REQUIRE(stmt->type == KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected AST flow control tree statement to be block"));

    REQUIRE_OK(kefir_ast_flow_control_structure_data_elements_append(mem, &stmt->value.block.data_elements, element));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_point_common_parent(struct kefir_ast_flow_control_point *point1,
                                                          struct kefir_ast_flow_control_point *point2,
                                                          struct kefir_ast_flow_control_structure **common_parent) {
    REQUIRE(point1 != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control point"));
    REQUIRE(point2 != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control point"));
    REQUIRE(common_parent != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST flow control structure"));

    *common_parent = NULL;
    struct kefir_ast_flow_control_structure *current_parent1 = point1->parent;
    while (current_parent1 != NULL && *common_parent == NULL) {
        struct kefir_ast_flow_control_structure *current_parent2 = point2->parent;
        while (current_parent2 != NULL && *common_parent == NULL) {
            if (current_parent2 == current_parent1) {
                *common_parent = current_parent2;
            } else {
                current_parent2 = current_parent2->parent_point != NULL ? current_parent2->parent_point->parent : NULL;
            }
        }
        if (*common_parent == NULL) {
            current_parent1 = current_parent1->parent_point != NULL ? current_parent1->parent_point->parent : NULL;
        }
    }
    REQUIRE(*common_parent != NULL,
            KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Unable to determine common parent for two flow control points"));

    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_point_parents(struct kefir_mem *mem, struct kefir_ast_flow_control_point *point,
                                                    struct kefir_list *parents,
                                                    struct kefir_ast_flow_control_structure *top_parent) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(point != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control point"));
    REQUIRE(parents != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST list"));

    struct kefir_ast_flow_control_structure *current_parent = point->parent;
    while (current_parent != top_parent && current_parent != NULL) {
        REQUIRE_OK(kefir_list_insert_after(mem, parents, kefir_list_tail(parents), current_parent));
        current_parent = current_parent->parent_point != NULL ? current_parent->parent_point->parent : NULL;
    }
    return KEFIR_OK;
}
