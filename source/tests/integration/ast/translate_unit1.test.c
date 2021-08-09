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

kefir_result_t make_unit(struct kefir_mem *mem, const struct kefir_ast_context *context,
                         struct kefir_ast_translation_unit **result) {
    struct kefir_ast_translation_unit *unit = kefir_ast_new_translation_unit(mem);

    struct kefir_ast_declaration_list *decl1 = kefir_ast_new_single_declaration_list(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "value"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))), NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &unit->external_definitions, kefir_list_tail(&unit->external_definitions),
                                       KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_declaration_list *decl2 = kefir_ast_new_single_declaration_list(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "value_to_add"), NULL, NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &unit->external_definitions, kefir_list_tail(&unit->external_definitions),
                                       KEFIR_AST_NODE_BASE(decl2)));

    struct kefir_ast_declaration *decl3_declaration = NULL;
    struct kefir_ast_declaration_list *decl3 = kefir_ast_new_single_declaration_list(
        mem,
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "value_callback")),
        NULL, &decl3_declaration);
    struct kefir_ast_declaration_list *decl3_param1 =
        kefir_ast_new_single_declaration_list(mem, kefir_ast_declarator_identifier(mem, NULL, NULL), NULL, NULL);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl3_param1->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &decl3_declaration->declarator->function.parameters,
                                       kefir_list_tail(&decl3_declaration->declarator->function.parameters),
                                       KEFIR_AST_NODE_BASE(decl3_param1)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &unit->external_definitions, kefir_list_tail(&unit->external_definitions),
                                       KEFIR_AST_NODE_BASE(decl3)));

    struct kefir_ast_compound_statement *func1_body = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_declarator *func1_decl =
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "set_value"));
    struct kefir_ast_declaration_list *func1_param1 = kefir_ast_new_single_declaration_list(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "new_value"), NULL, NULL);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &func1_param1->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &func1_decl->function.parameters,
                                       kefir_list_tail(&func1_decl->function.parameters),
                                       KEFIR_AST_NODE_BASE(func1_param1)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func1_body->block_items, kefir_list_tail(&func1_body->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                     mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value")),
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "new_value"))))))));
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(mem, func1_decl, func1_body);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &unit->external_definitions, kefir_list_tail(&unit->external_definitions),
                                       KEFIR_AST_NODE_BASE(func1)));

    struct kefir_ast_compound_statement *func2_body = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_declarator *func2_decl =
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_value"));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func2_body->block_items, kefir_list_tail(&func2_body->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value"))))));
    struct kefir_ast_function_definition *func2 = kefir_ast_new_function_definition(mem, func2_decl, func2_body);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func2->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &unit->external_definitions, kefir_list_tail(&unit->external_definitions),
                                       KEFIR_AST_NODE_BASE(func2)));

    struct kefir_ast_compound_statement *func3_body = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_declarator *func3_decl =
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "add_value"));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func3_body->block_items, kefir_list_tail(&func3_body->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_compound_assignment(
                     mem, KEFIR_AST_ASSIGNMENT_ADD,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value")),
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value_to_add"))))))));
    struct kefir_ast_function_definition *func3 = kefir_ast_new_function_definition(mem, func3_decl, func3_body);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func3->specifiers, kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &unit->external_definitions, kefir_list_tail(&unit->external_definitions),
                                       KEFIR_AST_NODE_BASE(func3)));

    struct kefir_ast_compound_statement *func4_body = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_declarator *func4_decl =
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "invoke_callback"));
    struct kefir_ast_function_call *func4_stmt1 = kefir_ast_new_function_call(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value_callback")));
    REQUIRE_OK(kefir_list_insert_after(mem, &func4_stmt1->arguments, kefir_list_tail(&func4_stmt1->arguments),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value"))));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func4_body->block_items, kefir_list_tail(&func4_body->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(mem, KEFIR_AST_NODE_BASE(func4_stmt1)))));
    struct kefir_ast_function_definition *func4 = kefir_ast_new_function_definition(mem, func4_decl, func4_body);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func4->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &unit->external_definitions, kefir_list_tail(&unit->external_definitions),
                                       KEFIR_AST_NODE_BASE(func4)));

    *result = unit;
    return KEFIR_OK;
}

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

    struct kefir_ast_translation_unit *unit = NULL;
    REQUIRE_OK(make_unit(mem, &global_context.context, &unit));

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(unit)));

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&global_translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translate_unit(mem, KEFIR_AST_NODE_BASE(unit), &global_translator_context));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &global_translator_context));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
