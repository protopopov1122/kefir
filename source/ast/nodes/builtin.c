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

#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_builtin_visit, kefir_ast_builtin, builtin)

struct kefir_ast_node_base *ast_builtin_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_builtin_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_builtin *, node, base->self);
    REQUIRE_OK(kefir_list_free(mem, &node->arguments));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_BUILTIN_CLASS = {
    .type = KEFIR_AST_BUILTIN, .visit = ast_builtin_visit, .clone = ast_builtin_clone, .free = ast_builtin_free};

static kefir_result_t builtin_argument_free(struct kefir_mem *mem, struct kefir_list *list,
                                            struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    return KEFIR_OK;
}

struct kefir_ast_node_base *ast_builtin_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_builtin *, node, base->self);
    struct kefir_ast_builtin *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_builtin));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_BUILTIN_CLASS;
    clone->base.self = clone;
    clone->base.source_location = base->source_location;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    res = kefir_list_init(&clone->arguments);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    res = kefir_list_on_remove(&clone->arguments, builtin_argument_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &clone->arguments);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->builtin = node->builtin;
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->arguments); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, iter->value);
        struct kefir_ast_node_base *arg_clone = KEFIR_AST_NODE_CLONE(mem, arg);
        REQUIRE_ELSE(arg_clone != NULL, {
            kefir_list_free(mem, &clone->arguments);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
        res = kefir_list_insert_after(mem, &clone->arguments, kefir_list_tail(&clone->arguments), arg_clone);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_list_free(mem, &clone->arguments);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    }
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_builtin *kefir_ast_new_builtin(struct kefir_mem *mem, kefir_ast_builtin_operator_t builtin_op) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_builtin *builtin = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_builtin));
    REQUIRE(builtin != NULL, NULL);
    builtin->base.klass = &AST_BUILTIN_CLASS;
    builtin->base.self = builtin;
    kefir_result_t res = kefir_ast_node_properties_init(&builtin->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, builtin);
        return NULL;
    });
    res = kefir_source_location_empty(&builtin->base.source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, builtin);
        return NULL;
    });
    builtin->builtin = builtin_op;
    res = kefir_list_init(&builtin->arguments);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, builtin);
        return NULL;
    });
    res = kefir_list_on_remove(&builtin->arguments, builtin_argument_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &builtin->arguments);
        KEFIR_FREE(mem, builtin);
        return NULL;
    });
    return builtin;
}

kefir_result_t kefir_ast_builtin_append(struct kefir_mem *mem, struct kefir_ast_builtin *call,
                                        struct kefir_ast_node_base *arg) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(call != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST builtin"));
    REQUIRE(arg != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid builtin parameter AST node"));
    REQUIRE_OK(kefir_list_insert_after(mem, &call->arguments, kefir_list_tail(&call->arguments), arg));
    return KEFIR_OK;
}
