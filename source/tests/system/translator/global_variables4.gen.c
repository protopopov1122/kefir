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

static kefir_result_t define_unit1(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_enum_specifier *specifier0 = kefir_ast_enum_specifier_init(mem, NULL, NULL, true);
    REQUIRE_OK(kefir_ast_enum_specifier_append(mem, specifier0, context->symbols, "MULTIVALUE_INT", NULL));
    REQUIRE_OK(kefir_ast_enum_specifier_append(mem, specifier0, context->symbols, "MULTIVALUE_FLOAT", NULL));
    REQUIRE_OK(kefir_ast_enum_specifier_append(mem, specifier0, context->symbols, "MULTIVALUE_PTR", NULL));

    struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(mem, NULL, NULL, true);

    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context->symbols, "int64"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry2->declaration.specifiers,
                                                          kefir_ast_type_specifier_double(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry2, kefir_ast_declarator_identifier(mem, context->symbols, "float64"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry2));

    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry3->declaration.specifiers,
                                                          kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry3, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "ptr")),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry3));

    struct kefir_ast_structure_specifier *specifier2 =
        kefir_ast_structure_specifier_init(mem, context->symbols, "multivalue", true);
    struct kefir_ast_structure_declaration_entry *entryA = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entryA->declaration.specifiers,
                                                          kefir_ast_type_specifier_enum(mem, specifier0)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entryA, kefir_ast_declarator_identifier(mem, context->symbols, "type"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier2, entryA));

    struct kefir_ast_structure_declaration_entry *entryB = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entryB->declaration.specifiers,
                                                          kefir_ast_type_specifier_union(mem, specifier1)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entryB, kefir_ast_declarator_identifier(mem, context->symbols, "value"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier2, entryB));

    struct kefir_ast_declaration *decl1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "multivalue_t"), NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_typedef(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_type_specifier_struct(mem, specifier2)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit2(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "MULTIVALUE_FLOAT")))));

    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list,
        kefir_ast_new_initializer_member_designation(
            mem, context->symbols, "float64",
            kefir_ast_new_initializer_member_designation(mem, context->symbols, "value", NULL)),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 8.163f)))));

    struct kefir_ast_declaration *decl1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "multi1"), init1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl1->specifiers, kefir_ast_type_specifier_typedef(mem, context->symbols, "multivalue_t")));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit3(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(mem);

    struct kefir_ast_initializer *init1_1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_1->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "MULTIVALUE_INT")))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_1->list,
        kefir_ast_new_initializer_member_designation(
            mem, context->symbols, "int64",
            kefir_ast_new_initializer_member_designation(mem, context->symbols, "value", NULL)),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0xffe)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init1->list, NULL, init1_1));

    struct kefir_ast_initializer *init1_2 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_2->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "MULTIVALUE_PTR")))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_2->list,
        kefir_ast_new_initializer_member_designation(
            mem, context->symbols, "ptr",
            kefir_ast_new_initializer_member_designation(mem, context->symbols, "value", NULL)),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "multi1")))))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init1->list, NULL, init1_2));

    struct kefir_ast_initializer *init1_3 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_3->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "MULTIVALUE_FLOAT")))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_3->list,
        kefir_ast_new_initializer_member_designation(
            mem, context->symbols, "float64",
            kefir_ast_new_initializer_member_designation(mem, context->symbols, "value", NULL)),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 6.67f)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init1->list, NULL, init1_3));

    struct kefir_ast_initializer *init1_4 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_4->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "MULTIVALUE_PTR")))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1_4->list,
        kefir_ast_new_initializer_member_designation(
            mem, context->symbols, "ptr",
            kefir_ast_new_initializer_member_designation(mem, context->symbols, "value", NULL)),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "multi2")))))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list,
        kefir_ast_new_initializer_index_designation(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)), NULL),
        init1_4));

    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
        mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                   kefir_ast_declarator_identifier(mem, context->symbols, "multiarr")),
        init1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl1->specifiers, kefir_ast_type_specifier_typedef(mem, context->symbols, "multivalue_t")));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit4(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_declaration *decl0 = kefir_ast_new_declaration(
        mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                   kefir_ast_declarator_identifier(mem, context->symbols, "multiarr")),
        NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl0->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl0->specifiers, kefir_ast_type_specifier_typedef(mem, context->symbols, "multivalue_t")));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl0)));

    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "MULTIVALUE_PTR")))));

    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list,
        kefir_ast_new_initializer_member_designation(
            mem, context->symbols, "ptr",
            kefir_ast_new_initializer_member_designation(mem, context->symbols, "value", NULL)),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "multiarr")))));

    struct kefir_ast_declaration *decl1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "multi2"), init1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl1->specifiers, kefir_ast_type_specifier_typedef(mem, context->symbols, "multivalue_t")));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                  struct kefir_list *unit) {
    REQUIRE_OK(define_unit1(mem, context, unit));
    REQUIRE_OK(define_unit2(mem, context, unit));
    REQUIRE_OK(define_unit4(mem, context, unit));
    REQUIRE_OK(define_unit3(mem, context, unit));
    return KEFIR_OK;
}

static kefir_result_t free_variable(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                    void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(entry != NULL, KEFIR_INTERNAL_ERROR);
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));

    struct kefir_list unit;
    REQUIRE_OK(kefir_list_init(&unit));
    REQUIRE_OK(kefir_list_on_remove(&unit, free_variable, NULL));

    REQUIRE_OK(define_unit(mem, &global_context.context, &unit));

    for (const struct kefir_list_entry *iter = kefir_list_head(&unit); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, node));
    }

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    translator_context.global_scope_layout = &global_scope;

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));

    for (const struct kefir_list_entry *iter = kefir_list_head(&unit); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, iter->value);
        if (node->properties.category == KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION) {
            REQUIRE_OK(kefir_ast_translate_function(mem, node, &translator_context));
        }
    }

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(kefir_list_free(mem, &unit));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
