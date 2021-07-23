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

#include "kefir/ir/type_tree.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t free_node(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key,
                                kefir_hashtree_value_t value, void *payload) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(payload);

    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(struct kefir_ir_type_tree_node *, node, value);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type tree node"));

    REQUIRE_OK(kefir_list_free(mem, &node->subtypes));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

struct traversal_payload {
    struct kefir_mem *mem;
    struct kefir_ir_type_tree *tree;
    struct kefir_ir_type_tree_node *parent;
    struct kefir_ir_type_visitor *visitor;
    kefir_size_t slot;
};

static struct kefir_ir_type_tree_node *alloc_node(struct kefir_mem *mem, struct kefir_ir_type_tree *tree,
                                                  struct kefir_ir_type_tree_node *parent, kefir_size_t index,
                                                  kefir_size_t relative_slot, kefir_size_t slot_width) {

    struct kefir_ir_type_tree_node *node = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_type_tree_node));
    REQUIRE(node != NULL, NULL);

    kefir_result_t res = kefir_list_init(&node->subtypes);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, node);
        return NULL;
    });

    node->index = index;
    node->parent = parent;
    node->relative_slot = relative_slot;
    node->slot_width = slot_width;
    node->type = tree->type;

    res = kefir_hashtree_insert(mem, &tree->index, (kefir_hashtree_key_t) index, (kefir_hashtree_value_t) node);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &node->subtypes);
        KEFIR_FREE(mem, node);
        return NULL;
    });

    if (parent != NULL) {
        res = kefir_list_insert_after(mem, &parent->subtypes, kefir_list_tail(&parent->subtypes), node);
    } else {
        res = kefir_list_insert_after(mem, &tree->roots, kefir_list_tail(&tree->roots), node);
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_delete(mem, &tree->index, (kefir_hashtree_key_t) index);
        kefir_list_free(mem, &node->subtypes);
        KEFIR_FREE(mem, node);
        return NULL;
    });

    return node;
}

static kefir_result_t visit_typeentry(const struct kefir_ir_type *type, kefir_size_t index,
                                      const struct kefir_ir_typeentry *typeentry, void *payload) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct traversal_payload *, param, payload);

    struct kefir_ir_type_tree_node *node = alloc_node(param->mem, param->tree, param->parent, index, param->slot, 1);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR type tree node"));
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_STRUCT:
        case KEFIR_IR_TYPE_UNION: {
            struct traversal_payload nested_param = {
                .mem = param->mem, .parent = node, .slot = 1, .tree = param->tree, .visitor = param->visitor};
            REQUIRE_OK(
                kefir_ir_type_visitor_list_nodes(type, param->visitor, &nested_param, index + 1, typeentry->param));
            node->slot_width = nested_param.slot;
        } break;

        case KEFIR_IR_TYPE_ARRAY: {
            struct traversal_payload nested_param = {
                .mem = param->mem, .parent = node, .slot = 1, .tree = param->tree, .visitor = param->visitor};
            REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, &nested_param, index + 1, 1));
            node->slot_width = (nested_param.slot - 1) * typeentry->param + 1;
        } break;

        default:
            break;
    }

    param->slot += node->slot_width;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_type_tree_init(struct kefir_mem *mem, const struct kefir_ir_type *type,
                                       struct kefir_ir_type_tree *tree) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type tree"));

    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visit_typeentry));

    tree->type = type;
    REQUIRE_OK(kefir_list_init(&tree->roots));
    REQUIRE_OK(kefir_hashtree_init(&tree->index, &kefir_hashtree_uint_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&tree->index, free_node, NULL));

    struct traversal_payload param = {.mem = mem, .parent = NULL, .slot = 0, .tree = tree, .visitor = &visitor};
    kefir_result_t res = kefir_ir_type_visitor_list_nodes(type, &visitor, &param, 0, kefir_ir_type_nodes(type));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &tree->index);
        kefir_list_free(mem, &tree->roots);
        return KEFIR_OK;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ir_type_tree_free(struct kefir_mem *mem, struct kefir_ir_type_tree *tree) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type tree"));

    REQUIRE_OK(kefir_list_free(mem, &tree->roots));
    REQUIRE_OK(kefir_hashtree_free(mem, &tree->index));
    tree->type = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_type_tree_at(const struct kefir_ir_type_tree *tree, kefir_size_t index,
                                     const struct kefir_ir_type_tree_node **node) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type tree"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to IR type tree node"));

    struct kefir_hashtree_node *tree_node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&tree->index, (kefir_hashtree_key_t) index, &tree_node));
    *node = (void *) tree_node->value;
    return KEFIR_OK;
}
