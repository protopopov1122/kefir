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

#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_struct_member_visit, kefir_ast_struct_member, struct_member)
NODE_VISIT_IMPL(ast_struct_indirect_member_visit, kefir_ast_struct_member, struct_indirect_member)

struct kefir_ast_node_base *ast_struct_member_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_struct_member_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_struct_member *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->structure));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_STRUCT_MEMBER_CLASS = {.type = KEFIR_AST_STRUCTURE_MEMBER,
                                                             .visit = ast_struct_member_visit,
                                                             .clone = ast_struct_member_clone,
                                                             .free = ast_struct_member_free};

const struct kefir_ast_node_class AST_STRUCT_INDIRECT_MEMBER_CLASS = {.type = KEFIR_AST_STRUCTURE_INDIRECT_MEMBER,
                                                                      .visit = ast_struct_indirect_member_visit,
                                                                      .clone = ast_struct_member_clone,
                                                                      .free = ast_struct_member_free};

struct kefir_ast_node_base *ast_struct_member_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_struct_member *, node, base->self);
    struct kefir_ast_struct_member *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_struct_member));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = node->base.klass;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->structure = KEFIR_AST_NODE_CLONE(mem, node->structure);
    REQUIRE_ELSE(clone->structure != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->member = node->member;
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_struct_member *kefir_ast_new_struct_member(struct kefir_mem *mem, struct kefir_symbol_table *symbols,
                                                            struct kefir_ast_node_base *structure, const char *member) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(symbols != NULL, NULL);
    REQUIRE(structure != NULL, NULL);
    REQUIRE(member != NULL, NULL);
    const char *member_copy = kefir_symbol_table_insert(mem, symbols, member, NULL);
    REQUIRE(member != NULL, NULL);
    struct kefir_ast_struct_member *struct_member = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_struct_member));
    REQUIRE(struct_member != NULL, NULL);
    struct_member->base.klass = &AST_STRUCT_MEMBER_CLASS;
    struct_member->base.self = struct_member;
    kefir_result_t res = kefir_ast_node_properties_init(&struct_member->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, struct_member);
        return NULL;
    });
    struct_member->structure = structure;
    struct_member->member = member_copy;
    return struct_member;
}

struct kefir_ast_struct_member *kefir_ast_new_struct_indirect_member(struct kefir_mem *mem,
                                                                     struct kefir_symbol_table *symbols,
                                                                     struct kefir_ast_node_base *structure,
                                                                     const char *member) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(symbols != NULL, NULL);
    REQUIRE(structure != NULL, NULL);
    REQUIRE(member != NULL, NULL);
    const char *member_copy = kefir_symbol_table_insert(mem, symbols, member, NULL);
    REQUIRE(member != NULL, NULL);
    struct kefir_ast_struct_member *struct_member = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_struct_member));
    REQUIRE(struct_member != NULL, NULL);
    struct_member->base.klass = &AST_STRUCT_INDIRECT_MEMBER_CLASS;
    struct_member->base.self = struct_member;
    kefir_result_t res = kefir_ast_node_properties_init(&struct_member->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, struct_member);
        return NULL;
    });
    struct_member->structure = structure;
    struct_member->member = member_copy;
    return struct_member;
}
