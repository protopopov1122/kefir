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

#include "kefir/core/tree.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t child_node_remove(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                        void *payload) {
    UNUSED(list);
    UNUSED(payload);
    ASSIGN_DECL_CAST(struct kefir_tree_node *, node, entry->value);
    REQUIRE_OK(kefir_tree_free(mem, node));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

kefir_result_t kefir_tree_init(struct kefir_tree_node *root, void *value) {
    REQUIRE(root != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node"));
    root->value = value;
    root->parent = NULL;
    root->prev_sibling = NULL;
    root->next_sibling = NULL;
    REQUIRE_OK(kefir_list_init(&root->children));
    REQUIRE_OK(kefir_list_on_remove(&root->children, child_node_remove, NULL));
    root->removal_callback = NULL;
    root->removal_payload = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_tree_free(struct kefir_mem *mem, struct kefir_tree_node *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node"));

    if (node->removal_callback != NULL) {
        REQUIRE_OK(node->removal_callback(mem, node->value, node->removal_payload));
    }
    REQUIRE_OK(kefir_list_free(mem, &node->children));
    return KEFIR_OK;
}

kefir_result_t kefir_tree_on_removal(struct kefir_tree_node *node,
                                     kefir_result_t (*callback)(struct kefir_mem *, void *, void *), void *payload) {
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node"));
    node->removal_callback = callback;
    node->removal_payload = payload;
    return KEFIR_OK;
}

kefir_result_t kefir_tree_insert_child(struct kefir_mem *mem, struct kefir_tree_node *node, void *value,
                                       struct kefir_tree_node **subnode) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node"));
    struct kefir_tree_node *child = KEFIR_MALLOC(mem, sizeof(struct kefir_tree_node));
    REQUIRE(child != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate tree node"));
    child->value = value;
    child->parent = node;
    child->removal_callback = node->removal_callback;
    child->removal_payload = node->removal_payload;
    kefir_result_t res = kefir_list_init(&child->children);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, child);
        return res;
    });
    res = kefir_list_on_remove(&child->children, child_node_remove, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &child->children);
        KEFIR_FREE(mem, child);
        return res;
    });
    struct kefir_list_entry *last_child = kefir_list_tail(&node->children);
    REQUIRE_OK(kefir_list_insert_after(mem, &node->children, last_child, child));
    child->prev_sibling = last_child != NULL ? (struct kefir_tree_node *) last_child->value : NULL;
    child->next_sibling = NULL;
    if (child->prev_sibling != NULL) {
        child->prev_sibling->next_sibling = child;
    }
    if (subnode != NULL) {
        *subnode = child;
    }
    return KEFIR_OK;
}

struct kefir_tree_node *kefir_tree_first_child(const struct kefir_tree_node *node) {
    REQUIRE(node != NULL, NULL);
    struct kefir_list_entry *head = kefir_list_head(&node->children);
    REQUIRE(head != NULL, NULL);
    return (struct kefir_tree_node *) head->value;
}

struct kefir_tree_node *kefir_tree_next_sibling(const struct kefir_tree_node *node) {
    REQUIRE(node != NULL, NULL);
    return node->next_sibling;
}

struct kefir_tree_node *kefir_tree_prev_sibling(const struct kefir_tree_node *node) {
    REQUIRE(node != NULL, NULL);
    return node->prev_sibling;
}

kefir_result_t kefir_tree_iter(struct kefir_tree_node *root, struct kefir_tree_node_iterator *iter) {
    REQUIRE(root != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node iterator"));
    iter->current = root;
    REQUIRE_OK(kefir_list_init(&iter->pending));
    return KEFIR_OK;
    ;
}

kefir_result_t kefir_tree_iter_next(struct kefir_mem *mem, struct kefir_tree_node_iterator *iter) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node iterator"));
    if (iter->current == NULL) {
        return KEFIR_OK;
    }
    for (struct kefir_tree_node *child = kefir_tree_first_child(iter->current); child != NULL;
         child = kefir_tree_next_sibling(child)) {
        REQUIRE_OK(kefir_list_insert_after(mem, &iter->pending, kefir_list_tail(&iter->pending), child));
    }
    struct kefir_list_entry *head = kefir_list_head(&iter->pending);
    if (head != NULL) {
        iter->current = (struct kefir_tree_node *) head->value;
        REQUIRE_OK(kefir_list_pop(mem, &iter->pending, head));
        return KEFIR_OK;
    } else {
        iter->current = NULL;
        REQUIRE_OK(kefir_list_free(mem, &iter->pending));
        return KEFIR_ITERATOR_END;
    }
}

kefir_result_t kefir_tree_iter_free(struct kefir_mem *mem, struct kefir_tree_node_iterator *iter) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node iterator"));
    iter->current = NULL;
    REQUIRE_OK(kefir_list_free(mem, &iter->pending));
    return KEFIR_OK;
    ;
}
