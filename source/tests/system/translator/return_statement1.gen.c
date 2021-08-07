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

#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "codegen.h"

kefir_result_t make_unit(struct kefir_mem *mem, const struct kefir_ast_context *context,
                         struct kefir_ast_translation_unit **result) {
    struct kefir_ast_translation_unit *unit = kefir_ast_new_translation_unit(mem);

    struct kefir_ast_compound_statement *func1_body = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_declarator *func1_decl =
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "int_to_float"));
    struct kefir_ast_declaration *func1_param1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "value"), NULL);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &func1_param1->specifiers, kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &func1_decl->function.parameters,
                                       kefir_list_tail(&func1_decl->function.parameters),
                                       KEFIR_AST_NODE_BASE(func1_param1)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func1_body->block_items, kefir_list_tail(&func1_body->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value"))))));
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(mem, func1_decl, func1_body);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &unit->external_definitions, kefir_list_tail(&unit->external_definitions),
                                       KEFIR_AST_NODE_BASE(func1)));

    struct kefir_ast_compound_statement *func2_body = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_declarator *func2_decl =
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "float_to_int"));
    struct kefir_ast_declaration *func2_param1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "value"), NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func2_param1->specifiers,
                                                          kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &func2_decl->function.parameters,
                                       kefir_list_tail(&func2_decl->function.parameters),
                                       KEFIR_AST_NODE_BASE(func2_param1)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func2_body->block_items, kefir_list_tail(&func2_body->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value"))))));
    struct kefir_ast_function_definition *func2 = kefir_ast_new_function_definition(mem, func2_decl, func2_body);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func2->specifiers, kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &unit->external_definitions, kefir_list_tail(&unit->external_definitions),
                                       KEFIR_AST_NODE_BASE(func2)));

    *result = unit;
    return KEFIR_OK;
}
static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));

    struct kefir_ast_translation_unit *unit = NULL;
    REQUIRE_OK(make_unit(mem, &global_context.context, &unit));

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(unit)));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    translator_context.global_scope_layout = &global_scope;

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(kefir_ast_translate_unit(mem, KEFIR_AST_NODE_BASE(unit), &translator_context));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
