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
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "codegen.h"

#include "codegen.inc.c"

static kefir_result_t define_compound_literal_function(struct kefir_mem *mem, struct function *func,
                                                       struct kefir_ast_context_manager *context_manager,
                                                       const char *name) {
    func->identifier = name;
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_structure_specifier *specifier0 = kefir_ast_structure_specifier_init(mem, NULL, NULL, true);
    struct kefir_ast_structure_declaration_entry *entryU1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entryU1->declaration.specifiers,
                                                          kefir_ast_type_specifier_double(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entryU1, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "fp64"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier0, entryU1));

    struct kefir_ast_structure_declaration_entry *entryU2 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entryU2->declaration.specifiers,
                                                          kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entryU2,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                   KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)),
                                   kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "fp32")),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier0, entryU2));

    struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(mem, NULL, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_qualifier_const(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                   KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 32)),
                                   kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "string")),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry2->declaration.specifiers,
                                                          kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry2->declaration.specifiers,
                                                          kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry2, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "length"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry2));

    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry3->declaration.specifiers,
                                                          kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry3->declaration.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry3->declaration.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &entry3->declaration.specifiers,
        kefir_ast_alignment_specifier(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 16)))));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry3, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "padding"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry3));

    struct kefir_ast_structure_declaration_entry *entry4 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry4->declaration.specifiers,
                                                          kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry4,
        kefir_ast_declarator_pointer(mem,
                                     kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "ptr")),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry4));

    struct kefir_ast_structure_declaration_entry *entry5 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry5->declaration.specifiers,
                                                          kefir_ast_type_specifier_union(mem, specifier0)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry5,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                   KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)),
                                   kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "floats")),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry5));

    struct kefir_ast_type_name *type_name1 =
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers,
                                                          kefir_ast_type_specifier_struct(mem, specifier1)));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context_manager->current, KEFIR_AST_NODE_BASE(type_name1)));

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle, type_name1->base.properties.type,
                                         &func_type);

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        name, func->type, NULL, NULL));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    const char *STRING = "Goodbye, world!";
    struct kefir_ast_compound_literal *compound = kefir_ast_new_compound_literal(mem, type_name1);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &compound->initializer->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(mem, STRING)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &compound->initializer->list,
        kefir_ast_new_initializer_member_designation(mem, context_manager->current->symbols, "padding", NULL),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1010)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &compound->initializer->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(mem, STRING)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &compound->initializer->list,
        kefir_ast_new_initializer_member_designation(mem, context_manager->current->symbols, "length", NULL),
        kefir_ast_new_expression_initializer(mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, strlen(STRING))))));

    struct kefir_ast_initializer *list1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &list1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 4.0013)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &list1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 19.88263)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &list1->list,
        kefir_ast_new_initializer_index_designation(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)),
            kefir_ast_new_initializer_member_designation(
                mem, context_manager->current->symbols, "fp32",
                kefir_ast_new_initializer_index_designation(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)), NULL))),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1.02f)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &list1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2.04f)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &list1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1e5)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &compound->initializer->list,
        kefir_ast_new_initializer_member_designation(mem, context_manager->current->symbols, "floats", NULL), list1));

    func->body = KEFIR_AST_NODE_BASE(compound);

    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct function compound1;
    REQUIRE_OK(define_compound_literal_function(mem, &compound1, &context_manager, "compound1"));

    REQUIRE_OK(analyze_function(mem, &compound1, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));

    REQUIRE_OK(translate_function(mem, &compound1, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(free_function(mem, &compound1));

    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
