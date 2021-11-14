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
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast-translator/value.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/downcast.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast-translator/function_definition.h"

static kefir_result_t init_function_declaration(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                struct kefir_ast_function_definition *function,
                                                struct kefir_ast_translator_function_context *args) {
    const char *identifier = NULL;
    REQUIRE_OK(kefir_ast_declarator_unpack_identifier(function->declarator, &identifier));
    REQUIRE(identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected function definition to have valid identifier"));

    const struct kefir_ast_declarator_function *decl_func = NULL;
    REQUIRE_OK(kefir_ast_declarator_unpack_function(function->declarator, &decl_func));
    REQUIRE(decl_func != NULL,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected function definition to have function declarator"));
    args->resolved_function_type = NULL;
    switch (function->base.properties.type->function_type.mode) {
        case KEFIR_AST_FUNCTION_TYPE_PARAMETERS:
        case KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY:
            REQUIRE_OK(kefir_ast_translator_function_declaration_init(
                mem, context->environment, context->ast_context->type_bundle, context->ast_context->type_traits,
                context->module, &context->type_cache.resolver, identifier,
                function->base.properties.function_definition.scoped_id->type, NULL, &args->function_declaration));
            break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS: {
            struct kefir_list declaration_list;
            struct kefir_hashtree declarations;

            kefir_result_t res = kefir_hashtree_init(&declarations, &kefir_hashtree_str_ops);
            for (const struct kefir_list_entry *iter = kefir_list_head(&function->declarations);
                 res == KEFIR_OK && iter != NULL; kefir_list_next(&iter)) {

                ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl_node, iter->value);
                struct kefir_ast_declaration *decl_list = NULL;
                REQUIRE_MATCH(&res, kefir_ast_downcast_declaration(decl_node, &decl_list, false),
                              KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected AST declaration"));
                ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl,
                                 kefir_list_head(&decl_list->init_declarators)->value);
                REQUIRE_CHAIN(
                    &res, kefir_hashtree_insert(mem, &declarations,
                                                (kefir_hashtree_key_t) decl->properties.declaration_props.identifier,
                                                (kefir_hashtree_value_t) decl));
            }

            REQUIRE_CHAIN(&res, kefir_list_init(&declaration_list));
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_hashtree_free(mem, &declarations);
                return res;
            });

            for (const struct kefir_list_entry *iter = kefir_list_head(&decl_func->parameters);
                 res == KEFIR_OK && iter != NULL; kefir_list_next(&iter)) {

                struct kefir_hashtree_node *tree_node = NULL;
                ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param, iter->value);
                REQUIRE_CHAIN_SET(&res,
                                  param->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION &&
                                      param->properties.expression_props.identifier != NULL,
                                  KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected parameter to be AST identifier"));
                REQUIRE_CHAIN(&res,
                              kefir_hashtree_at(&declarations,
                                                (kefir_hashtree_key_t) param->properties.expression_props.identifier,
                                                &tree_node));

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
                context->module, NULL, identifier, function->base.properties.type, &declaration_list,
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

static kefir_result_t free_function_declaration(struct kefir_mem *mem,
                                                struct kefir_ast_translator_function_context *args) {
    if (args->resolved_function_type == NULL) {
        REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, args->function_declaration));
    }
    args->resolved_function_type = NULL;
    args->function_declaration = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_function_context_init(struct kefir_mem *mem,
                                                          struct kefir_ast_translator_context *context,
                                                          struct kefir_ast_function_definition *function,
                                                          struct kefir_ast_translator_function_context *ctx) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator context"));
    REQUIRE(function != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST function definition"));
    REQUIRE(ctx != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST translator function context"));

    REQUIRE_OK(init_function_declaration(mem, context, function, ctx));

    ctx->function_definition = function;
    ctx->module = context->module;
    ctx->local_context = function->base.properties.function_definition.scoped_id->function.local_context;
    kefir_result_t res =
        kefir_ast_translator_context_init_local(&ctx->local_translator_context, &ctx->local_context->context, context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        free_function_declaration(mem, ctx);
        return res;
    });

    res = kefir_ast_translator_local_scope_layout_init(mem, context->module, context->global_scope_layout,
                                                       &ctx->local_scope_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_context_free(mem, &ctx->local_translator_context);
        free_function_declaration(mem, ctx);
        return res;
    });

    res = kefir_ast_translator_build_local_scope_layout(
        mem, ctx->local_context, ctx->local_translator_context.environment, ctx->local_translator_context.module,
        &ctx->local_translator_context.type_cache.resolver, &ctx->local_scope_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &ctx->local_scope_layout);
        kefir_ast_translator_context_free(mem, &ctx->local_translator_context);
        free_function_declaration(mem, ctx);
        return res;
    });

    res = kefir_ast_translator_flow_control_tree_init(mem, ctx->local_context->context.flow_control_tree);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &ctx->local_scope_layout);
        kefir_ast_translator_context_free(mem, &ctx->local_translator_context);
        free_function_declaration(mem, ctx);
        return res;
    });

    ctx->ir_func = kefir_ir_module_new_function(mem, context->module, ctx->function_declaration->ir_function_decl,
                                                ctx->local_scope_layout.local_layout, 0);
    REQUIRE_ELSE(ctx->ir_func != NULL, {
        kefir_ast_translator_local_scope_layout_free(mem, &ctx->local_scope_layout);
        kefir_ast_translator_context_free(mem, &ctx->local_translator_context);
        free_function_declaration(mem, ctx);
        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate IR function");
    });

    res = kefir_irbuilder_block_init(mem, &ctx->builder, &ctx->ir_func->body);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &ctx->local_scope_layout);
        kefir_ast_translator_context_free(mem, &ctx->local_translator_context);
        free_function_declaration(mem, ctx);
        return res;
    });

    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_function_context_free(struct kefir_mem *mem,
                                                          struct kefir_ast_translator_function_context *ctx) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(ctx != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator function context"));

    kefir_result_t res = KEFIR_IRBUILDER_BLOCK_FREE(&ctx->builder);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &ctx->local_scope_layout);
        kefir_ast_translator_context_free(mem, &ctx->local_translator_context);
        free_function_declaration(mem, ctx);
        return res;
    });

    res = kefir_ast_translator_local_scope_layout_free(mem, &ctx->local_scope_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_context_free(mem, &ctx->local_translator_context);
        free_function_declaration(mem, ctx);
        return res;
    });

    res = kefir_ast_translator_context_free(mem, &ctx->local_translator_context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        free_function_declaration(mem, ctx);
        return res;
    });

    REQUIRE_OK(free_function_declaration(mem, ctx));
    ctx->function_definition = NULL;
    return KEFIR_OK;
}

static kefir_result_t xchg_param_address(struct kefir_irbuilder_block *builder, const struct kefir_ast_type *type) {
    if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(type)) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 2));
    }
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 1));
    return KEFIR_OK;
}

struct vl_modified_param {
    struct kefir_mem *mem;
    struct kefir_irbuilder_block *builder;
    struct kefir_ast_translator_context *context;
};

static kefir_result_t translate_variably_modified(const struct kefir_ast_node_base *node, void *payload) {
    REQUIRE(node != NULL, KEFIR_OK);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct vl_modified_param *, param, payload);

    REQUIRE_OK(kefir_ast_translate_expression(param->mem, node, param->builder, param->context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(param->builder, KEFIR_IROPCODE_POP, 0));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_function_context_translate(
    struct kefir_mem *mem, struct kefir_ast_translator_function_context *function_context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(function_context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator function context"));

    struct kefir_ast_function_definition *function = function_context->function_definition;
    struct kefir_irbuilder_block *builder = &function_context->builder;
    struct kefir_ast_translator_context *context = &function_context->local_translator_context;

    struct kefir_ast_local_context *local_context =
        function->base.properties.function_definition.scoped_id->function.local_context;
    const struct kefir_ast_declarator_function *decl_func = NULL;
    REQUIRE_OK(kefir_ast_declarator_unpack_function(function->declarator, &decl_func));
    REQUIRE(decl_func != NULL,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected function definition to have function declarator"));
    kefir_result_t res;
    for (const struct kefir_list_entry *iter = kefir_list_tail(&decl_func->parameters); iter != NULL;
         iter = iter->prev) {

        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param, iter->value);
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;

        if (param->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION) {
            struct kefir_ast_declaration *param_decl = NULL;
            REQUIRE_MATCH_OK(
                &res, kefir_ast_downcast_declaration(param, &param_decl, false),
                KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected function parameter to be an AST declaration"));
            REQUIRE(kefir_list_length(&param_decl->init_declarators) == 1,
                    KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected function parameter to have exactly one declarator"));

            ASSIGN_DECL_CAST(struct kefir_ast_init_declarator *, init_decl,
                             kefir_list_head(&param_decl->init_declarators)->value);
            const char *param_identifier = NULL;
            REQUIRE_OK(kefir_ast_declarator_unpack_identifier(init_decl->declarator, &param_identifier));
            REQUIRE(init_decl->base.properties.type != NULL,
                    KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Function definition parameters shall have definite types"));
            if (init_decl->base.properties.type->tag != KEFIR_AST_TYPE_VOID && param_identifier != NULL) {
                REQUIRE_OK(local_context->context.resolve_ordinary_identifier(&local_context->context, param_identifier,
                                                                              &scoped_id));
                REQUIRE_OK(kefir_ast_translator_object_lvalue(mem, context, builder, param_identifier, scoped_id));
                REQUIRE_OK(xchg_param_address(builder, scoped_id->object.type));
                REQUIRE_OK(kefir_ast_translator_store_value(mem, scoped_id->type, context, builder));
            } else {
                if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(init_decl->base.properties.type)) {
                    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
                }
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
            }

            REQUIRE_OK(kefir_ast_type_list_variable_modificators(
                init_decl->base.properties.type, translate_variably_modified,
                &(struct vl_modified_param){.mem = mem, .context = context, .builder = builder}));
        } else if (param->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION &&
                   param->properties.expression_props.identifier != NULL) {
            REQUIRE_OK(local_context->context.resolve_ordinary_identifier(
                &local_context->context, param->properties.expression_props.identifier, &scoped_id));
            REQUIRE_OK(kefir_ast_translator_object_lvalue(mem, context, builder,
                                                          param->properties.expression_props.identifier, scoped_id));
            REQUIRE_OK(xchg_param_address(builder, scoped_id->object.type));
            REQUIRE_OK(kefir_ast_translator_store_value(mem, scoped_id->object.type, context, builder));
        } else {
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                   "Expected function parameter to be either AST declaration or identifier");
        }
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&function_context->function_definition->declarations);
         iter != NULL; kefir_list_next(&iter)) {

        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl_node, iter->value);
        struct kefir_ast_declaration *decl_list = NULL;
        struct kefir_ast_init_declarator *decl = NULL;
        REQUIRE_MATCH_OK(&res, kefir_ast_downcast_declaration(decl_node, &decl_list, false),
                         KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected AST node to be a declaration"));
        REQUIRE_OK(kefir_ast_declaration_unpack_single(decl_list, &decl));

        REQUIRE_OK(kefir_ast_type_list_variable_modificators(
            decl->base.properties.type, translate_variably_modified,
            &(struct vl_modified_param){.mem = mem, .context = context, .builder = builder}));
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&function->body->block_items); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item, iter->value);

        if (item->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT) {
            REQUIRE_OK(kefir_ast_translate_statement(mem, item, builder, context));
        } else if (item->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION ||
                   item->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR) {
            REQUIRE_OK(kefir_ast_translate_declaration(mem, item, builder, context));
        } else {
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected compound statement item");
        }
    }

    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_function_context_finalize(
    struct kefir_mem *mem, struct kefir_ast_translator_function_context *function_context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(function_context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator function context"));

    REQUIRE_OK(kefir_ast_translate_local_scope(mem, &function_context->local_context->context, function_context->module,
                                               &function_context->local_scope_layout));
    return KEFIR_OK;
}
