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

#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/function_definition.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t free_function_context(struct kefir_mem *mem, struct kefir_list *list,
                                            struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_translator_function_context *, function_context, entry->value);
    REQUIRE_OK(kefir_ast_translator_function_context_free(mem, function_context));
    KEFIR_FREE(mem, function_context);
    return KEFIR_OK;
}

static kefir_result_t allocate_function_context(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                struct kefir_ast_node_base *external_definition,
                                                struct kefir_ast_translator_function_context **function_context) {
    REQUIRE(external_definition->properties.category == KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION &&
                external_definition->klass->type == KEFIR_AST_FUNCTION_DEFINITION,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected function definition AST node"));

    ASSIGN_DECL_CAST(struct kefir_ast_function_definition *, function_definition, external_definition->self);

    *function_context = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_function_context));
    REQUIRE(function_context != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator function context"));

    kefir_result_t res =
        kefir_ast_translator_function_context_init(mem, context, function_definition, *function_context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, *function_context);
        *function_context = NULL;
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t translate_unit_impl(struct kefir_mem *mem, struct kefir_ast_translation_unit *unit,
                                          struct kefir_ast_translator_context *context,
                                          struct kefir_list *function_translator_contexts) {
    for (const struct kefir_list_entry *iter = kefir_list_head(&unit->external_definitions); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, external_definition, iter->value);

        switch (external_definition->properties.category) {
            case KEFIR_AST_NODE_CATEGORY_DECLARATION:
            case KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR:
                // Intentionally left blank
                break;

            case KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION: {
                struct kefir_ast_translator_function_context *func_ctx = NULL;
                REQUIRE_OK(allocate_function_context(mem, context, external_definition, &func_ctx));
                kefir_result_t res = kefir_list_insert_after(mem, function_translator_contexts,
                                                             kefir_list_tail(function_translator_contexts), func_ctx);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    kefir_ast_translator_function_context_free(mem, func_ctx);
                    KEFIR_FREE(mem, func_ctx);
                    return res;
                });

                REQUIRE_OK(kefir_ast_translator_function_context_translate(mem, func_ctx));
            } break;

            default:
                return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected external definition node category");
        }
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(function_translator_contexts); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_translator_function_context *, func_ctx, iter->value);
        REQUIRE_OK(kefir_ast_translator_function_context_finalize(mem, func_ctx));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_unit(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                        struct kefir_ast_translator_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(node != NULL && node->properties.category == KEFIR_AST_NODE_CATEGORY_TRANSLATION_UNIT &&
                node->klass->type == KEFIR_AST_TRANSLATION_UNIT,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation unit"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator context"));

    ASSIGN_DECL_CAST(struct kefir_ast_translation_unit *, unit, node->self);
    struct kefir_list function_translator_contexts;
    REQUIRE_OK(kefir_list_init(&function_translator_contexts));
    REQUIRE_OK(kefir_list_on_remove(&function_translator_contexts, free_function_context, NULL));

    kefir_result_t res = translate_unit_impl(mem, unit, context, &function_translator_contexts);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &function_translator_contexts);
        return res;
    });
    REQUIRE_OK(kefir_list_free(mem, &function_translator_contexts));
    return KEFIR_OK;
}
