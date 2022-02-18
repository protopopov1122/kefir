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

#include "kefir/core/block_tree.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"

static kefir_result_t block_removal(struct kefir_mem *mem, struct kefir_hashtree *hashtree, kefir_hashtree_key_t key,
                                    kefir_hashtree_value_t value, void *payload) {
    UNUSED(hashtree);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_block_tree *, tree, payload);
    ASSIGN_DECL_CAST(void *, block, value);
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block pointer"));

    if (tree->on_block_removal.callback != NULL) {
        REQUIRE_OK(
            tree->on_block_removal.callback(mem, tree, (kefir_size_t) key, block, tree->on_block_removal.payload));
    }
    KEFIR_FREE(mem, block);
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_init(struct kefir_block_tree *tree, kefir_size_t block_size) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree"));
    REQUIRE(block_size > 0, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected non-zero block size"));

    REQUIRE_OK(kefir_hashtree_init(&tree->tree, &kefir_hashtree_uint_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&tree->tree, block_removal, tree));
    tree->block_size = block_size;
    tree->on_block_init = (struct kefir_block_tree_event){0};
    tree->on_block_removal = (struct kefir_block_tree_event){0};
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_free(struct kefir_mem *mem, struct kefir_block_tree *tree) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree"));

    REQUIRE_OK(kefir_hashtree_free(mem, &tree->tree));
    *tree = (struct kefir_block_tree){0};
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_on_block_init(struct kefir_block_tree *tree, kefir_block_tree_callback_t callback,
                                              void *payload) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree"));

    tree->on_block_init.callback = callback;
    tree->on_block_init.payload = payload;
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_on_block_removal(struct kefir_block_tree *tree, kefir_block_tree_callback_t callback,
                                                 void *payload) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree"));

    tree->on_block_removal.callback = callback;
    tree->on_block_removal.payload = payload;
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_get_block_offset(const struct kefir_block_tree *tree, kefir_size_t position,
                                                 kefir_size_t *block, kefir_size_t *block_offset) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree"));

    ASSIGN_PTR(block, position / tree->block_size);
    ASSIGN_PTR(block_offset, position % tree->block_size);
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_get_block(const struct kefir_block_tree *tree, kefir_size_t block_id,
                                          void **block_ptr) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree"));
    REQUIRE(block_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to block"));

    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&tree->tree, (kefir_hashtree_key_t) block_id, &node));
    *block_ptr = (void *) node->value;
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_block(struct kefir_mem *mem, struct kefir_block_tree *tree, kefir_size_t block_id,
                                      void **block_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree"));
    REQUIRE(block_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to block"));

    kefir_result_t res = kefir_block_tree_get_block(tree, block_id, block_ptr);
    if (res == KEFIR_NOT_FOUND) {
        void *block = KEFIR_MALLOC(mem, tree->block_size);

        if (tree->on_block_init.callback != NULL) {
            res = tree->on_block_init.callback(mem, tree, block_id, block, tree->on_block_init.payload);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_FREE(mem, block);
                return res;
            });
        }

        res = kefir_hashtree_insert(mem, &tree->tree, (kefir_hashtree_key_t) block_id, (kefir_hashtree_value_t) block);
        REQUIRE_ELSE(res == KEFIR_OK, {
            if (tree->on_block_removal.callback != NULL) {
                tree->on_block_removal.callback(mem, tree, block_id, block, tree->on_block_removal.payload);
            }
            KEFIR_FREE(mem, block);
            return res;
        });

        *block_ptr = block;
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_iter(const struct kefir_block_tree *tree, struct kefir_block_tree_iterator *iter) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree iterator"));

    kefir_hashtree_iter(&tree->tree, &iter->tree_iter);
    if (iter->tree_iter.node != NULL) {
        iter->block_id = iter->tree_iter.node->key;
        iter->block = (void *) iter->tree_iter.node->value;
    } else {
        iter->block = NULL;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_next(struct kefir_block_tree_iterator *iter) {
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree iterator"));

    kefir_hashtree_next(&iter->tree_iter);
    if (iter->tree_iter.node != NULL) {
        iter->block_id = iter->tree_iter.node->key;
        iter->block = (void *) iter->tree_iter.node->value;
    } else {
        iter->block = NULL;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_block_tree_iter_skip_to(const struct kefir_block_tree *tree,
                                             struct kefir_block_tree_iterator *iter, kefir_size_t position) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid block tree iterator"));

    while (iter->block != NULL && (iter->block_id + 1) * tree->block_size <= position) {
        REQUIRE_OK(kefir_block_tree_next(iter));
    }
    return KEFIR_OK;
}
