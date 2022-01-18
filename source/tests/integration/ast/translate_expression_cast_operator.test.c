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

#include "kefir/core/mem.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/test/util.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/format.h"
#include <stdio.h>
#include <stdarg.h>

static kefir_result_t append_specifiers(struct kefir_mem *mem, struct kefir_ast_declarator_specifier_list *list,
                                        int count, ...) {
    va_list args;
    va_start(args, count);
    while (count--) {
        struct kefir_ast_declarator_specifier *specifier = va_arg(args, struct kefir_ast_declarator_specifier *);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, list, specifier));
    }
    va_end(args);
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_util_default_type_traits(), &env.target_env, &global_context, NULL));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

#define CAST_NODE(_type_name, _node)                                                           \
    do {                                                                                       \
        struct kefir_ast_node_base *node1 =                                                    \
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem, (_type_name), (_node)));      \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node1));                               \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node1, &builder, &translator_context)); \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node1));                                           \
    } while (0)

    struct kefir_ast_struct_type *struct1_type = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(mem, context->type_bundle, "", &struct1_type);
    REQUIRE_OK(
        kefir_ast_struct_type_field(mem, context->symbols, struct1_type, "field_one", kefir_ast_type_char(), NULL));

    REQUIRE_OK(kefir_ast_global_context_declare_external(
        mem, &global_context, "ptr", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void()), NULL,
        NULL, NULL));
    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context, "structure", type1, NULL, NULL, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(
        kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context, context, &env, &module, NULL));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
        mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));

    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *func1_params = kefir_ir_module_new_type(mem, &module, 0, &func_params);
    struct kefir_ir_type *func1_returns = kefir_ir_module_new_type(mem, &module, 0, &func_returns);
    REQUIRE(func1_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(func1_returns != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function_decl *func1_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func1", func_params, false, func_returns);
    REQUIRE(func1_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func1 =
        kefir_ir_module_new_function(mem, &module, func1_decl, translator_local_scope.local_layout_id, 0);
    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func1->body));

#define MAKE_TYPENAME(_id, _spec_count, ...)                                                                          \
    struct kefir_ast_type_name *_id = kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)); \
    REQUIRE_OK(append_specifiers(mem, &_id->type_decl.specifiers, (_spec_count), __VA_ARGS__));

    MAKE_TYPENAME(type_name1, 1, kefir_ast_type_specifier_boolean(mem));
    MAKE_TYPENAME(type_name2, 1, kefir_ast_type_specifier_int(mem));
    MAKE_TYPENAME(type_name3, 1, kefir_ast_type_specifier_int(mem));
    MAKE_TYPENAME(type_name4, 1, kefir_ast_type_specifier_short(mem));
    MAKE_TYPENAME(type_name5, 1, kefir_ast_type_specifier_long(mem));
    MAKE_TYPENAME(type_name6, 1, kefir_ast_type_specifier_long(mem));
    MAKE_TYPENAME(type_name7, 2, kefir_ast_type_specifier_long(mem), kefir_ast_type_specifier_long(mem));
    MAKE_TYPENAME(type_name8, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_int(mem));
    MAKE_TYPENAME(type_name9, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_short(mem));
    MAKE_TYPENAME(type_name10, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_long(mem));
    MAKE_TYPENAME(type_name11, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_long(mem));
    MAKE_TYPENAME(type_name12, 3, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_long(mem),
                  kefir_ast_type_specifier_long(mem));
    MAKE_TYPENAME(type_name13, 1, kefir_ast_type_specifier_float(mem));
    MAKE_TYPENAME(type_name14, 1, kefir_ast_type_specifier_float(mem));
    MAKE_TYPENAME(type_name15, 1, kefir_ast_type_specifier_float(mem));
    MAKE_TYPENAME(type_name16, 1, kefir_ast_type_specifier_float(mem));
    MAKE_TYPENAME(type_name17, 1, kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name18, 1, kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name19, 1, kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name20, 1, kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name21, 1, kefir_ast_type_specifier_void(mem));
    MAKE_TYPENAME(type_name22, 1, kefir_ast_type_specifier_void(mem));
    MAKE_TYPENAME(type_name23, 1, kefir_ast_type_specifier_void(mem));
    MAKE_TYPENAME(type_name24, 1, kefir_ast_type_specifier_void(mem));
    MAKE_TYPENAME(type_name25, 1, kefir_ast_type_specifier_void(mem));
    MAKE_TYPENAME(type_name26, 1, kefir_ast_type_specifier_void(mem));
    MAKE_TYPENAME(type_name30, 2, kefir_ast_type_specifier_long(mem), kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name31, 2, kefir_ast_type_specifier_long(mem), kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name32, 2, kefir_ast_type_specifier_long(mem), kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name33, 2, kefir_ast_type_specifier_long(mem), kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name34, 2, kefir_ast_type_specifier_long(mem), kefir_ast_type_specifier_long(mem));
    MAKE_TYPENAME(type_name35, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_int(mem));
    MAKE_TYPENAME(type_name36, 1, kefir_ast_type_specifier_float(mem));
    MAKE_TYPENAME(type_name37, 1, kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name38, 1, kefir_ast_type_specifier_void(mem));
#undef MAKE_TYPENAME

    CAST_NODE(type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, -150)));
    CAST_NODE(type_name2, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100)));
    CAST_NODE(type_name3, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true)));
    CAST_NODE(type_name4, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 100)));
    CAST_NODE(type_name5, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 3.14f)));
    CAST_NODE(type_name6, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 3.14)));
    CAST_NODE(type_name7, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr")));

    CAST_NODE(type_name8, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1234)));
    CAST_NODE(type_name9, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 101)));
    CAST_NODE(type_name10, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2.71f)));
    CAST_NODE(type_name11, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 2.71)));
    CAST_NODE(type_name12, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr")));

    CAST_NODE(type_name13, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -67)));
    CAST_NODE(type_name14, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 67)));
    CAST_NODE(type_name15, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.069f)));
    CAST_NODE(type_name16, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 42.0)));

    CAST_NODE(type_name17, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100500)));
    CAST_NODE(type_name18, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1597)));
    CAST_NODE(type_name19, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.0112f)));
    CAST_NODE(type_name20, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 42.1)));

    struct kefir_ast_type_name *type_name27 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name27->type_decl.specifiers,
                                                          kefir_ast_type_specifier_short(mem)));

    struct kefir_ast_type_name *type_name28 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name28->type_decl.specifiers,
                                                          kefir_ast_type_specifier_boolean(mem)));

    struct kefir_ast_type_name *type_name29 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name29->type_decl.specifiers,
                                                          kefir_ast_type_specifier_float(mem)));

    CAST_NODE(type_name27, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -0xfee)));
    CAST_NODE(type_name28, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 0xcade)));
    CAST_NODE(type_name29, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr")));

    CAST_NODE(type_name21, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10000)));
    CAST_NODE(type_name22, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 10001)));
    CAST_NODE(type_name23, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 16.7f)));
    CAST_NODE(type_name24, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 982.0001)));
    CAST_NODE(type_name25, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr")));
    CAST_NODE(type_name26, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "structure")));

    CAST_NODE(type_name30, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -8716)));
    CAST_NODE(type_name31, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 1)));
    CAST_NODE(type_name32, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, -18.12f)));
    CAST_NODE(type_name33, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 9381.18)));
    CAST_NODE(type_name34, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, 1000.16162l)));
    CAST_NODE(type_name35, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, -1.05e-9l)));
    CAST_NODE(type_name36, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, -0.17253e10l)));
    CAST_NODE(type_name37, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, 0.01e-1l)));
    CAST_NODE(type_name38, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, 1234.0l)));

#undef CAST_NODE
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
