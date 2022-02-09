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
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_util_default_type_traits(), &env.target_env, &global_context, NULL));
    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context, "variable", kefir_ast_type_signed_int(),
                                                         NULL, NULL, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));

    struct kefir_ast_translator_context global_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &global_translator_context, &global_context.context, &env,
                                                 &module, NULL));
    global_translator_context.global_scope_layout = &translator_global_scope;

    struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(mem, NULL, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                   KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)),
                                   kefir_ast_declarator_identifier(mem, global_context.context.symbols, "content")),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry1));

    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 6)))));

    struct kefir_ast_initializer *init1_1 = kefir_ast_new_list_initializer(mem);
    struct kefir_ast_initializer *init1_1_1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_1_1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_1_1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -2)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init1_1->list, NULL, init1_1_1));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init1->list, NULL, init1_1));

    struct kefir_ast_initializer *init1_2 = kefir_ast_new_list_initializer(mem);
    struct kefir_ast_initializer *init1_2_1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_2_1->list,
        kefir_ast_new_initializer_index_designation(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)), NULL),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 11)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_2_1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 12)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init1_2->list, NULL, init1_2_1));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list,
        kefir_ast_new_initializer_index_designation(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)), NULL),
        init1_2));

    struct kefir_ast_initializer *init1_3 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_3->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 16)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_3->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 17)))));
    REQUIRE_OK(
        kefir_ast_initializer_list_append(mem, &init1->list,
                                          kefir_ast_new_initializer_member_designation(
                                              mem, global_context.context.symbols, "content",
                                              kefir_ast_new_initializer_index_designation(
                                                  mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)), NULL)),
                                          init1_3));

    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list,
        kefir_ast_new_initializer_index_designation(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)),
            kefir_ast_new_initializer_member_designation(
                mem, global_context.context.symbols, "content",
                kefir_ast_new_initializer_index_designation(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)), NULL))),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 19)))));

    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 20)))));

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                   kefir_ast_declarator_identifier(mem, global_context.context.symbols, "array")),
        init1, NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl1->specifiers,
        kefir_ast_type_specifier_struct(mem, kefir_ast_structure_specifier_clone(mem, specifier1))));

    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list,
        kefir_ast_new_initializer_index_designation(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)),
            kefir_ast_new_initializer_member_designation(
                mem, global_context.context.symbols, "content",
                kefir_ast_new_initializer_index_designation(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)), NULL))),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 6)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 7)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 8)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 9)))));

    struct kefir_ast_declaration *decl2 = kefir_ast_new_single_declaration(
        mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                   kefir_ast_declarator_identifier(mem, global_context.context.symbols, "array2")),
        init2, NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers,
                                                          kefir_ast_type_specifier_struct(mem, specifier1)));

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl1)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl2)));

    REQUIRE_OK(
        kefir_ast_translator_build_global_scope_layout(mem, &module, &global_context, &env, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl1)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl2)));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &global_translator_context));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
