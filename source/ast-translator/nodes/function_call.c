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

#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast-translator/function_declaration.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t translate_parameters(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                           struct kefir_irbuilder_block *builder,
                                           const struct kefir_ast_function_call *node,
                                           struct kefir_ast_translator_function_declaration *func_decl) {
    const struct kefir_list_entry *arg_value_iter = kefir_list_head(&node->arguments);
    const struct kefir_list_entry *decl_arg_iter = kefir_list_head(&func_decl->argument_layouts);
    for (; arg_value_iter != NULL && decl_arg_iter != NULL;
         kefir_list_next(&arg_value_iter), kefir_list_next(&decl_arg_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, parameter_layout, decl_arg_iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, parameter_value, arg_value_iter->value);

        const struct kefir_ast_type *param_init_normalized_type =
            kefir_ast_translator_normalize_type(parameter_value->properties.type);
        const struct kefir_ast_type *param_normalized_type =
            KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle, param_init_normalized_type);
        REQUIRE(param_normalized_type != NULL,
                KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));

        REQUIRE_OK(kefir_ast_translate_expression(mem, parameter_value, builder, context));
        if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(param_normalized_type)) {
            REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits, param_normalized_type,
                                                    parameter_layout->type));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_function_call_node(struct kefir_mem *mem,
                                                      struct kefir_ast_translator_context *context,
                                                      struct kefir_irbuilder_block *builder,
                                                      const struct kefir_ast_function_call *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST function call node"));

    const struct kefir_ast_type *function_type = kefir_ast_translator_normalize_type(node->function->properties.type);
    if (function_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        function_type = function_type->referenced_type;
    }
    REQUIRE(function_type->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected callable function"));

    const char *function_name = NULL;
    if (node->function->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION &&
        node->function->properties.expression_props.identifier != NULL &&
        node->function->properties.expression_props.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION &&
        node->function->properties.expression_props.scoped_id->function.storage ==
            KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN) {
        function_name = node->function->properties.expression_props.identifier;
    }

    struct kefir_ir_function_decl *ir_decl = NULL;
    switch (function_type->function_type.mode) {
        case KEFIR_AST_FUNCTION_TYPE_PARAMETERS: {
            struct kefir_ast_translator_function_declaration *func_decl = NULL;
            REQUIRE_OK(kefir_ast_translator_function_declaration_init(
                mem, context->environment, context->ast_context->type_bundle, context->ast_context->type_traits,
                context->module, NULL, function_name, function_type, &node->arguments, &func_decl));
            ir_decl = func_decl->ir_function_decl;
            if (ir_decl->name == NULL) {
                REQUIRE_OK(kefir_ast_translate_expression(mem, node->function, builder, context));
            }
            kefir_result_t res = translate_parameters(mem, context, builder, node, func_decl);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_translator_function_declaration_free(mem, func_decl);
                return res;
            });
            REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, func_decl));
        } break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS: {
            struct kefir_ast_translator_function_declaration *func_decl = NULL;
            REQUIRE_OK(kefir_ast_translator_function_declaration_init(
                mem, context->environment, context->ast_context->type_bundle, context->ast_context->type_traits,
                context->module, NULL, function_name, function_type, &node->arguments, &func_decl));
            ir_decl = func_decl->ir_function_decl;
            if (ir_decl->name == NULL) {
                REQUIRE_OK(kefir_ast_translate_expression(mem, node->function, builder, context));
            }
            kefir_result_t res = translate_parameters(mem, context, builder, node, func_decl);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_translator_function_declaration_free(mem, func_decl);
                return res;
            });
            REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, func_decl));
        } break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY: {
            struct kefir_ast_translator_function_declaration *func_decl = NULL;
            REQUIRE_OK(kefir_ast_translator_function_declaration_init(
                mem, context->environment, context->ast_context->type_bundle, context->ast_context->type_traits,
                context->module, NULL, function_name, function_type, &node->arguments, &func_decl));
            ir_decl = func_decl->ir_function_decl;
            if (ir_decl->name == NULL) {
                REQUIRE_OK(kefir_ast_translate_expression(mem, node->function, builder, context));
            }
            kefir_result_t res = translate_parameters(mem, context, builder, node, func_decl);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_translator_function_declaration_free(mem, func_decl);
                return res;
            });
            REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, func_decl));
        } break;
    }

    if (ir_decl->name == NULL) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_INVOKEV, ir_decl->id));
    } else {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_INVOKE, ir_decl->id));
    }

    if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(node->base.properties.type)) {
        REQUIRE_OK(kefir_ast_translate_typeconv_normalize(builder, context->ast_context->type_traits,
                                                          node->base.properties.type));
    }
    return KEFIR_OK;
}
