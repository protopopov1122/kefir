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

#include "kefir/ast-translator/flow_control.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"

struct dependant {
    struct kefir_irblock *block;
    kefir_size_t index;
};

static kefir_result_t dependant_free(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                     void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct dependant *, dep, entry->value);
    KEFIR_FREE(mem, dep);
    return KEFIR_OK;
}

static kefir_result_t point_cleanup(struct kefir_mem *mem, struct kefir_ast_flow_control_point *point, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(point != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control point"));

    struct kefir_ast_translator_flow_control_point *ast_translator_point =
        *((struct kefir_ast_translator_flow_control_point **) point->ptr);
    if (!ast_translator_point->resolved) {
        REQUIRE_OK(kefir_list_free(mem, &ast_translator_point->dependents));
    }
    ast_translator_point->resolved = false;
    KEFIR_FREE(mem, ast_translator_point);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_flow_control_point_init(
    struct kefir_mem *mem, struct kefir_ast_flow_control_point *point,
    struct kefir_ast_translator_flow_control_point **translator_point) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(point != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control point"));

    struct kefir_ast_translator_flow_control_point *ast_translator_point =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_flow_control_point));
    REQUIRE(ast_translator_point != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator flow control point"));

    ast_translator_point->resolved = false;
    kefir_result_t res = kefir_list_init(&ast_translator_point->dependents);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, ast_translator_point);
        return res;
    });

    res = kefir_list_on_remove(&ast_translator_point->dependents, dependant_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &ast_translator_point->dependents);
        KEFIR_FREE(mem, ast_translator_point);
        return res;
    });

    KEFIR_AST_FLOW_CONTROL_SET_CLEANUP(point, point_cleanup, NULL);

    ASSIGN_DECL_CAST(struct kefir_ast_translator_flow_control_point **, translator_point_ptr, point->ptr);
    *translator_point_ptr = ast_translator_point;
    ASSIGN_PTR(translator_point, ast_translator_point);
    return KEFIR_OK;
}

static kefir_result_t patch_command(struct kefir_irblock *block, kefir_size_t index, kefir_uint64_t value) {
    struct kefir_irinstr *instr = kefir_irblock_at(block, index);
    switch (instr->opcode) {
        case KEFIR_IROPCODE_JMP:
        case KEFIR_IROPCODE_BRANCH:
            instr->arg.u64 = value;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to patch IR instruction");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_flow_control_point_reference(struct kefir_mem *mem,
                                                                 struct kefir_ast_flow_control_point *point,
                                                                 struct kefir_irblock *block, kefir_size_t index) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(point != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control point"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block"));
    REQUIRE(index < kefir_irblock_length(block),
            KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Expected valid index in the IR block"));

    struct kefir_ast_translator_flow_control_point *ast_translator_point =
        *((struct kefir_ast_translator_flow_control_point **) point->ptr);

    if (ast_translator_point->resolved) {
        REQUIRE_OK(patch_command(block, index, ast_translator_point->address));
    } else {
        struct dependant *dep = KEFIR_MALLOC(mem, sizeof(struct dependant));
        REQUIRE(dep != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE,
                                             "Failed to allocate AST translator flow control point dependant"));
        dep->block = block;
        dep->index = index;
        kefir_result_t res = kefir_list_insert_after(mem, &ast_translator_point->dependents,
                                                     kefir_list_tail(&ast_translator_point->dependents), dep);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, dep);
            return res;
        });
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_flow_control_point_resolve(struct kefir_mem *mem,
                                                               struct kefir_ast_flow_control_point *point,
                                                               kefir_uint64_t value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(point != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control point"));

    struct kefir_ast_translator_flow_control_point *ast_translator_point =
        *((struct kefir_ast_translator_flow_control_point **) point->ptr);
    REQUIRE(!ast_translator_point->resolved,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot resolve already resolved AST translator flow contron point"));

    for (const struct kefir_list_entry *iter = kefir_list_head(&ast_translator_point->dependents); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct dependant *, dep, iter->value);
        REQUIRE_OK(patch_command(dep->block, dep->index, value));
    }
    REQUIRE_OK(kefir_list_free(mem, &ast_translator_point->dependents));
    ast_translator_point->resolved = true;
    ast_translator_point->address = value;
    return KEFIR_OK;
}

static kefir_result_t flow_control_tree_init(struct kefir_mem *mem, struct kefir_tree_node *node) {
    if (node->value != NULL) {
        ASSIGN_DECL_CAST(struct kefir_ast_flow_control_statement *, stmt, node->value);
        switch (stmt->type) {
            case KEFIR_AST_FLOW_CONTROL_STATEMENT_IF:
                REQUIRE_OK(
                    kefir_ast_translator_flow_control_point_init(mem, stmt->value.conditional.thenBranchEnd, NULL));
                if (stmt->value.conditional.elseBranchEnd != NULL) {
                    REQUIRE_OK(
                        kefir_ast_translator_flow_control_point_init(mem, stmt->value.conditional.elseBranchEnd, NULL));
                }
                break;

            case KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH: {
                struct kefir_hashtree_node_iterator iter;
                for (const struct kefir_hashtree_node *node =
                         kefir_hashtree_iter(&stmt->value.switchStatement.cases, &iter);
                     node != NULL; node = kefir_hashtree_next(&iter)) {
                    ASSIGN_DECL_CAST(struct kefir_ast_flow_control_point *, point, node->value);
                    REQUIRE_OK(kefir_ast_translator_flow_control_point_init(mem, point, NULL));
                }
                if (stmt->value.switchStatement.defaultCase != NULL) {
                    REQUIRE_OK(kefir_ast_translator_flow_control_point_init(
                        mem, stmt->value.switchStatement.defaultCase, NULL));
                }
                REQUIRE_OK(kefir_ast_translator_flow_control_point_init(mem, stmt->value.switchStatement.end, NULL));
            } break;

            case KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR:
            case KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE:
            case KEFIR_AST_FLOW_CONTROL_STATEMENT_DO:
                REQUIRE_OK(kefir_ast_translator_flow_control_point_init(mem, stmt->value.loop.continuation, NULL));
                REQUIRE_OK(kefir_ast_translator_flow_control_point_init(mem, stmt->value.loop.end, NULL));
                break;
        }
    }

    for (struct kefir_tree_node *child = kefir_tree_first_child(node); child != NULL;
         child = kefir_tree_next_sibling(child)) {
        REQUIRE_OK(flow_control_tree_init(mem, child));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_flow_control_tree_init(struct kefir_mem *mem,
                                                           struct kefir_ast_flow_control_tree *tree) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control tree"));

    REQUIRE_OK(flow_control_tree_init(mem, &tree->root));
    return KEFIR_OK;
}
