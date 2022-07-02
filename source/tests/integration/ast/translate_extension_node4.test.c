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

#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/ir/builder.h"
#include "kefir/ast/node.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ir/format.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"

static kefir_result_t analyze_extension_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                             struct kefir_ast_node_base *node) {
    UNUSED(mem);
    UNUSED(context);
    node->properties.category = KEFIR_AST_NODE_CATEGORY_DECLARATION;
    node->properties.type = kefir_ast_type_signed_int();
    node->properties.expression_props.lvalue = true;
    return KEFIR_OK;
}

static kefir_result_t translate_extension_node(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                               const struct kefir_ast_extension_node *node,
                                               struct kefir_irbuilder_block *builder,
                                               kefir_ast_translator_context_extension_tag_t tag) {
    UNUSED(mem);
    UNUSED(node);
    UNUSED(context);
    REQUIRE(tag == KEFIR_AST_TRANSLATOR_CONTEXT_EXTENSION_TAG_DECLARATION, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_GETLOCALS, 100));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IADD1, 1));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_STORE64, 100));
    return KEFIR_OK;
}

static kefir_result_t before_translate(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                       const struct kefir_ast_node_base *node, struct kefir_irbuilder_block *builder,
                                       kefir_ast_translator_context_extension_tag_t tag,
                                       struct kefir_ast_visitor *visitor) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(node);
    UNUSED(builder);
    if (tag == KEFIR_AST_TRANSLATOR_CONTEXT_EXTENSION_TAG_DECLARATION) {
        REQUIRE(visitor == NULL, KEFIR_INTERNAL_ERROR);
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64, 2));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64, 2));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64, 2));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IMUL, 0));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IADD, 0));
    }
    return KEFIR_OK;
}

static kefir_result_t after_translate(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                      const struct kefir_ast_node_base *node, struct kefir_irbuilder_block *builder,
                                      kefir_ast_translator_context_extension_tag_t tag) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(node);
    if (tag == KEFIR_AST_TRANSLATOR_CONTEXT_EXTENSION_TAG_DECLARATION) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64, 3));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64, 3));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64, 3));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IMUL, 0));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IADD, 0));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 0, &func_params),
                         *decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func1", func_params, false, func_returns);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl, KEFIR_ID_NONE, 0);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, decl_result, KEFIR_IR_TYPE_INT, 0, 0));

    struct kefir_ast_context_extensions analysis_ext = {.analyze_extension_node = analyze_extension_node};
    struct kefir_ast_translator_context_extensions translator_ext = {
        .translate_extension_node = translate_extension_node,
        .before_translate = before_translate,
        .after_translate = after_translate};

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_util_default_type_traits(),
                                             &kft_util_get_translator_environment()->target_env, &global_context,
                                             &analysis_ext));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context, &local_context.context,
                                                 kft_util_get_translator_environment(), &module, &translator_ext));

    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body));

    struct kefir_ast_extension_node_class ext_node_class = {0};

    struct kefir_ast_extension_node *ast = kefir_ast_new_extension_node(mem, &ext_node_class, NULL);
    REQUIRE_OK(kefir_ast_analyze_node(mem, &local_context.context, KEFIR_AST_NODE_BASE(ast)));
    REQUIRE_OK(kefir_ast_translate_declaration(mem, KEFIR_AST_NODE_BASE(ast), &builder, &translator_context));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(ast)));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    REQUIRE_OK(kefir_ir_format_module(stdout, &module));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}
