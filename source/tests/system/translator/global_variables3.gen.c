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
    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(mem, context->symbols, "matrix", true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1,
        kefir_ast_declarator_array(
            mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)),
            kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)),
                                       kefir_ast_declarator_identifier(mem, context->symbols, "table"))),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry2->declaration.specifiers,
                                                          kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry2, kefir_ast_declarator_identifier(mem, context->symbols, "dim"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry2));

    struct kefir_ast_declaration *decl1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "matrix_t"), NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_typedef(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_type_specifier_struct(mem, specifier1)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));
    return KEFIR_OK;
}

static kefir_result_t make_init(struct kefir_mem *mem, const struct kefir_ast_context *context, kefir_int64_t offset,
                                struct kefir_ast_initializer **result) {
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, kefir_ast_new_initializer_member_designation(mem, context->symbols, "dim", NULL),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5 + offset)))));

    struct kefir_ast_initializer *table1_init = kefir_ast_new_list_initializer(mem);
    struct kefir_ast_initializer *row1_1_init = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_1_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_1_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_1_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_1_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_1_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &table1_init->list, NULL, row1_1_init));

    struct kefir_ast_initializer *row1_3_init = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_3_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_3_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 11 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_3_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 12 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_3_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 13 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_3_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 14 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &table1_init->list,
        kefir_ast_new_initializer_index_designation(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)), NULL),
        row1_3_init));

    struct kefir_ast_initializer *row1_5_init = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_5_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 20 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_5_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 21 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_5_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 22 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_5_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 23 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &row1_5_init->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 24 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &table1_init->list,
        kefir_ast_new_initializer_index_designation(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)), NULL),
        row1_5_init));

    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, kefir_ast_new_initializer_member_designation(mem, context->symbols, "table", NULL),
        table1_init));

    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list,
        kefir_ast_new_initializer_index_designation(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)),
            kefir_ast_new_initializer_index_designation(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)),
                kefir_ast_new_initializer_member_designation(mem, context->symbols, "table", NULL))),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 15 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 16 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 17 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 18 + offset)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 19 + offset)))));
    *result = init1;
    return KEFIR_OK;
}

static kefir_result_t define_unit2(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_initializer *init1 = NULL;
    REQUIRE_OK(make_init(mem, context, 0, &init1));

    struct kefir_ast_declaration *decl1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "matrix1"), init1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl1->specifiers, kefir_ast_type_specifier_typedef(mem, context->symbols, "matrix_t")));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit3(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_initializer *init1 = NULL;
    REQUIRE_OK(make_init(mem, context, 10, &init1));

    struct kefir_ast_declaration *decl1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "matrix2"), init1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl1->specifiers, kefir_ast_type_specifier_typedef(mem, context->symbols, "matrix_t")));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem,
            kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_matrix2"))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &func1->specifiers,
        kefir_ast_type_specifier_struct(mem,
                                        kefir_ast_structure_specifier_init(mem, context->symbols, "matrix", false))));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "matrix2"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit4(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_initializer *init1 = NULL;
    REQUIRE_OK(make_init(mem, context, 100, &init1));

    struct kefir_ast_declaration *decl1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "matrix3"), init1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl1->specifiers, kefir_ast_type_specifier_typedef(mem, context->symbols, "matrix_t")));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem,
            kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_matrix3"))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &func1->specifiers,
        kefir_ast_type_specifier_struct(mem,
                                        kefir_ast_structure_specifier_init(mem, context->symbols, "matrix", false))));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "matrix3"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit5(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_initializer *init1 = NULL;
    REQUIRE_OK(make_init(mem, context, -1, &init1));

    struct kefir_ast_declaration *decl1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "matrix4"), init1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl1->specifiers, kefir_ast_type_specifier_typedef(mem, context->symbols, "matrix_t")));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem,
            kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_matrix4"))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &func1->specifiers,
        kefir_ast_type_specifier_struct(mem,
                                        kefir_ast_structure_specifier_init(mem, context->symbols, "matrix", false))));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "matrix4"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit6(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "get_matrix2")))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "get_matrix3")))));
    REQUIRE_OK(kefir_ast_initializer_list_append(
        mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "get_matrix4")))));

    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
        mem,
        kefir_ast_declarator_pointer(
            mem, kefir_ast_declarator_function(
                     mem, kefir_ast_declarator_pointer(
                              mem, kefir_ast_declarator_array(
                                       mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                       kefir_ast_declarator_identifier(mem, context->symbols, "matrices"))))),
        init1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &decl1->specifiers, kefir_ast_type_specifier_typedef(mem, context->symbols, "matrix_t")));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                  struct kefir_list *unit) {
    REQUIRE_OK(define_unit1(mem, context, unit));
    REQUIRE_OK(define_unit2(mem, context, unit));
    REQUIRE_OK(define_unit3(mem, context, unit));
    REQUIRE_OK(define_unit4(mem, context, unit));
    REQUIRE_OK(define_unit5(mem, context, unit));
    REQUIRE_OK(define_unit6(mem, context, unit));
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
