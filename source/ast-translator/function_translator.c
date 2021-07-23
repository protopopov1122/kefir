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

#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast-translator/value.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct function_translator_ctx {
    const struct kefir_ast_translator_resolved_type *resolved_function_type;
    struct kefir_ast_translator_function_declaration *function_declaration;
    struct kefir_ast_local_context *local_context;
    struct kefir_ast_translator_context local_translator_context;
    struct kefir_ast_translator_local_scope_layout local_scope_layout;
    struct kefir_ir_function *ir_func;
    struct kefir_irbuilder_block builder;
};

static kefir_result_t init_function_declaration(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                struct kefir_ast_function_definition *function,
                                                struct function_translator_ctx *args) {
    args->resolved_function_type = NULL;
    switch (function->base.properties.type->function_type.mode) {
        case KEFIR_AST_FUNCTION_TYPE_PARAMETERS:
        case KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY:
            REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_FUNCTION(
                mem, &context->type_cache.resolver, context->environment, context->ast_context->type_bundle,
                context->ast_context->type_traits, context->module,
                function->base.properties.function_definition.scoped_id->type, &args->resolved_function_type));
            args->function_declaration = args->resolved_function_type->function.declaration;
            break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS: {
            struct kefir_list declaration_list;
            struct kefir_hashtree declarations;

            kefir_result_t res = kefir_hashtree_init(&declarations, &kefir_hashtree_str_ops);
            for (const struct kefir_list_entry *iter = kefir_list_head(&function->declarations);
                 res == KEFIR_OK && iter != NULL; kefir_list_next(&iter)) {

                ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl, iter->value);
                res = kefir_hashtree_insert(mem, &declarations,
                                            (kefir_hashtree_key_t) decl->properties.declaration_props.identifier,
                                            (kefir_hashtree_value_t) decl);
            }

            REQUIRE_CHAIN(&res, kefir_list_init(&declaration_list));
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_hashtree_free(mem, &declarations);
                return res;
            });

            for (const struct kefir_list_entry *iter =
                     kefir_list_head(&function->base.properties.type->function_type.parameters);
                 res == KEFIR_OK && iter != NULL; kefir_list_next(&iter)) {

                ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, param, iter->value);

                struct kefir_hashtree_node *tree_node = NULL;
                res = kefir_hashtree_at(&declarations, (kefir_hashtree_key_t) param->identifier, &tree_node);

                REQUIRE_CHAIN(&res, kefir_list_insert_after(mem, &declaration_list, kefir_list_tail(&declaration_list),
                                                            (struct kefir_ast_node_base *) tree_node->value));
            }

            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_list_free(mem, &declaration_list);
                kefir_hashtree_free(mem, &declarations);
                return res;
            });

            res = kefir_ast_translator_function_declaration_init(
                mem, context->environment, context->ast_context->type_bundle, context->ast_context->type_traits,
                context->module, &context->type_cache.resolver, function->base.properties.type, &declaration_list,
                &args->function_declaration);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_list_free(mem, &declaration_list);
                kefir_hashtree_free(mem, &declarations);
                return res;
            });

            res = kefir_list_free(mem, &declaration_list);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_translator_function_declaration_free(mem, args->function_declaration);
                kefir_hashtree_free(mem, &declarations);
                return res;
            });

            res = kefir_hashtree_free(mem, &declarations);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_translator_function_declaration_free(mem, args->function_declaration);
                return res;
            });
        } break;
    }

    return KEFIR_OK;
}

static kefir_result_t free_function_declaration(struct kefir_mem *mem, struct function_translator_ctx *args) {
    if (args->resolved_function_type == NULL) {
        REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, args->function_declaration));
    }
    args->resolved_function_type = NULL;
    args->function_declaration = NULL;
    return KEFIR_OK;
}

static kefir_result_t function_translator_ctx_init(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                   struct kefir_ast_function_definition *function,
                                                   struct function_translator_ctx *args) {

    REQUIRE_OK(init_function_declaration(mem, context, function, args));

    args->local_context = function->base.properties.function_definition.scoped_id->function.local_context;
    kefir_result_t res = kefir_ast_translator_context_init_local(&args->local_translator_context,
                                                                 &args->local_context->context, context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        free_function_declaration(mem, args);
        return res;
    });

    res = kefir_ast_translator_local_scope_layout_init(mem, context->module, context->global_scope_layout,
                                                       &args->local_scope_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_context_free(mem, &args->local_translator_context);
        free_function_declaration(mem, args);
        return res;
    });

    res = kefir_ast_translator_build_local_scope_layout(
        mem, args->local_context, args->local_translator_context.environment, args->local_translator_context.module,
        &args->local_translator_context.type_cache.resolver, &args->local_scope_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &args->local_scope_layout);
        kefir_ast_translator_context_free(mem, &args->local_translator_context);
        free_function_declaration(mem, args);
        return res;
    });

    res = kefir_ast_translator_flow_control_tree_init(mem, args->local_context->context.flow_control_tree);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &args->local_scope_layout);
        kefir_ast_translator_context_free(mem, &args->local_translator_context);
        free_function_declaration(mem, args);
        return res;
    });

    args->ir_func = kefir_ir_module_new_function(mem, context->module, args->function_declaration->ir_function_decl,
                                                 args->local_scope_layout.local_layout, 0);
    REQUIRE_ELSE(args->ir_func != NULL, {
        kefir_ast_translator_local_scope_layout_free(mem, &args->local_scope_layout);
        kefir_ast_translator_context_free(mem, &args->local_translator_context);
        free_function_declaration(mem, args);
        return KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate IR function");
    });

    res = kefir_irbuilder_block_init(mem, &args->builder, &args->ir_func->body);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &args->local_scope_layout);
        kefir_ast_translator_context_free(mem, &args->local_translator_context);
        free_function_declaration(mem, args);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t function_translator_ctx_free(struct kefir_mem *mem, struct function_translator_ctx *args) {
    kefir_result_t res = KEFIR_IRBUILDER_BLOCK_FREE(&args->builder);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &args->local_scope_layout);
        kefir_ast_translator_context_free(mem, &args->local_translator_context);
        free_function_declaration(mem, args);
        return res;
    });

    res = kefir_ast_translator_local_scope_layout_free(mem, &args->local_scope_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_context_free(mem, &args->local_translator_context);
        free_function_declaration(mem, args);
        return res;
    });

    res = kefir_ast_translator_context_free(mem, &args->local_translator_context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        free_function_declaration(mem, args);
        return res;
    });

    REQUIRE_OK(free_function_declaration(mem, args));
    return KEFIR_OK;
}

static kefir_result_t translate_function_impl(struct kefir_mem *mem, struct kefir_ast_function_definition *function,
                                              struct kefir_irbuilder_block *builder,
                                              struct kefir_ast_translator_context *context) {
    struct kefir_ast_local_context *local_context =
        function->base.properties.function_definition.scoped_id->function.local_context;
    for (const struct kefir_list_entry *iter =
             kefir_list_tail(&function->base.properties.type->function_type.parameters);
         iter != NULL; iter = iter->prev) {

        ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, param, iter->value);
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;

        if (param->identifier != NULL) {
            REQUIRE_OK(local_context->context.resolve_ordinary_identifier(&local_context->context, param->identifier,
                                                                          &scoped_id));
            REQUIRE_OK(kefir_ast_translator_object_lvalue(mem, context, builder, param->identifier, scoped_id));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 1));
            REQUIRE_OK(kefir_ast_translator_store_value(mem, scoped_id->type, context, builder));
        } else {
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
        }
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&function->body->block_items); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item, iter->value);

        if (item->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT) {
            REQUIRE_OK(kefir_ast_translate_statement(mem, item, builder, context));
        } else if (item->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION) {
            REQUIRE_OK(kefir_ast_translate_declaration(mem, item, builder, context));
        } else {
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected compound statement item");
        }
    }

    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_function(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                            struct kefir_ast_translator_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(context->global_scope_layout != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator global scope layout"));
    REQUIRE(node->properties.category == KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION &&
                node->klass->type == KEFIR_AST_FUNCTION_DEFINITION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected function definition AST node"));

    ASSIGN_DECL_CAST(struct kefir_ast_function_definition *, function, node->self);

    struct function_translator_ctx ctx;
    REQUIRE_OK(function_translator_ctx_init(mem, context, function, &ctx));

    kefir_result_t res = translate_function_impl(mem, function, &ctx.builder, context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        function_translator_ctx_free(mem, &ctx);
        return res;
    });

    REQUIRE_OK(function_translator_ctx_free(mem, &ctx));
    return KEFIR_OK;
}
