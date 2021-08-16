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
#include "./expression.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context, "variable", kefir_ast_type_signed_int(),
                                                         NULL, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));

    struct kefir_ast_translator_context global_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&global_translator_context, &global_context.context, &env, &module));
    global_translator_context.global_scope_layout = &translator_global_scope;

    struct kefir_ast_initializer *init1 = kefir_ast_new_expression_initializer(
        mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "Hello, world!")));

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                   kefir_ast_declarator_identifier(mem, global_context.context.symbols, "str1")),
        init1, NULL);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_qualifier_const(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_char(mem)));

    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "Goodbye, world!")))));

    struct kefir_ast_declaration *decl2 = kefir_ast_new_single_declaration(
        mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                   kefir_ast_declarator_identifier(mem, global_context.context.symbols, "str2")),
        init2, NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_qualifier_const(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_char(mem)));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(mem,
                                             KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "WTF world?")))));

    struct kefir_ast_declaration *decl3 = kefir_ast_new_single_declaration(
        mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                   KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 15)),
                                   kefir_ast_declarator_identifier(mem, global_context.context.symbols, "str3")),
        init3, NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers, kefir_ast_type_qualifier_const(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers, kefir_ast_type_specifier_char(mem)));

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl1)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl2)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl3)));

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&global_translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl1)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl2)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl3)));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &global_translator_context));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
