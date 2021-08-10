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

#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/node.h"

DEFINE_CASE(ast_nodes_goto_statements1, "AST nodes - goto statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_goto_statement *goto1 = kefir_ast_new_goto_statement(&kft_mem, &symbols, "label1");
    ASSERT(goto1 != NULL);
    ASSERT(goto1->base.klass->type == KEFIR_AST_GOTO_STATEMENT);
    ASSERT(goto1->base.self == goto1);
    ASSERT(goto1->identifier != NULL);
    ASSERT(strcmp(goto1->identifier, "label1") == 0);

    struct kefir_ast_goto_statement *goto2 = kefir_ast_new_goto_statement(&kft_mem, &symbols, "label2");
    ASSERT(goto2 != NULL);
    ASSERT(goto2->base.klass->type == KEFIR_AST_GOTO_STATEMENT);
    ASSERT(goto2->base.self == goto2);
    ASSERT(goto2->identifier != NULL);
    ASSERT(strcmp(goto2->identifier, "label2") == 0);

    struct kefir_ast_goto_statement *goto3 = kefir_ast_new_goto_statement(&kft_mem, NULL, "label3");
    ASSERT(goto3 != NULL);
    ASSERT(goto3->base.klass->type == KEFIR_AST_GOTO_STATEMENT);
    ASSERT(goto3->base.self == goto3);
    ASSERT(goto3->identifier != NULL);
    ASSERT(strcmp(goto3->identifier, "label3") == 0);

    struct kefir_ast_goto_statement *goto4 = kefir_ast_new_goto_statement(&kft_mem, NULL, NULL);
    ASSERT(goto4 == NULL);
    struct kefir_ast_goto_statement *goto5 = kefir_ast_new_goto_statement(&kft_mem, &symbols, NULL);
    ASSERT(goto5 == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(goto1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(goto2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(goto3)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_continue_statements, "AST nodes - continue statements") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_continue_statement *continue1 = kefir_ast_new_continue_statement(&kft_mem);
    ASSERT(continue1 != NULL);
    ASSERT(continue1->base.klass->type == KEFIR_AST_CONTINUE_STATEMENT);
    ASSERT(continue1->base.self == continue1);

    struct kefir_ast_continue_statement *continue2 = kefir_ast_new_continue_statement(&kft_mem);
    ASSERT(continue2 != NULL);
    ASSERT(continue2->base.klass->type == KEFIR_AST_CONTINUE_STATEMENT);
    ASSERT(continue2->base.self == continue2);
    ASSERT(continue1 != continue2);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(continue1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(continue2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_break_statements, "AST nodes - break statements") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_break_statement *break1 = kefir_ast_new_break_statement(&kft_mem);
    ASSERT(break1 != NULL);
    ASSERT(break1->base.klass->type == KEFIR_AST_BREAK_STATEMENT);
    ASSERT(break1->base.self == break1);

    struct kefir_ast_break_statement *break2 = kefir_ast_new_break_statement(&kft_mem);
    ASSERT(break2 != NULL);
    ASSERT(break2->base.klass->type == KEFIR_AST_BREAK_STATEMENT);
    ASSERT(break2->base.self == break2);
    ASSERT(break1 != break2);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(break1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(break2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_return_statements1, "AST nodes - return statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_return_statement *return1 =
        kefir_ast_new_return_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)));
    ASSERT(return1 != NULL);
    ASSERT(return1->base.klass->type == KEFIR_AST_RETURN_STATEMENT);
    ASSERT(return1->base.self == return1);
    ASSERT(return1->expression != NULL);
    ASSERT(return1->expression->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) return1->expression->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) return1->expression->self)->value.integer == 1);

    struct kefir_ast_return_statement *return2 = kefir_ast_new_return_statement(&kft_mem, NULL);
    ASSERT(return2 != NULL);
    ASSERT(return2->base.klass->type == KEFIR_AST_RETURN_STATEMENT);
    ASSERT(return2->base.self == return2);
    ASSERT(return2->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(return1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(return2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_function_definitions1, "AST nodes - function definitions #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_declaration_list *param1 = kefir_ast_new_single_declaration_list(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, &symbols, "x"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));

    struct kefir_ast_declaration_list *param2 = kefir_ast_new_single_declaration_list(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, &symbols, "y"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                         kefir_ast_type_specifier_double(&kft_mem)));

    struct kefir_ast_declarator *decl1 =
        kefir_ast_declarator_function(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, &symbols, "fn1"));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters,
                                      kefir_list_tail(&decl1->function.parameters), param1));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &body1->block_items, kefir_list_tail(&body1->block_items),
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL))));

    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(&kft_mem, decl1, body1);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &func1->declarations, kefir_list_tail(&func1->declarations), param2));

    ASSERT(func1 != NULL);
    ASSERT(func1->base.klass->type == KEFIR_AST_FUNCTION_DEFINITION);
    ASSERT(func1->base.self == func1);

    struct kefir_ast_declarator_specifier *specifier1 = NULL;
    struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(&func1->specifiers, &specifier1);
    ASSERT(iter != NULL);
    ASSERT(specifier1->klass == KEFIR_AST_TYPE_SPECIFIER);
    ASSERT(specifier1->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_INT);
    ASSERT_OK(kefir_ast_declarator_specifier_list_next(&iter, &specifier1));
    ASSERT(iter == NULL);

    ASSERT(func1->declarator == decl1);
    const struct kefir_list_entry *iter2 = kefir_list_head(&func1->declarations);
    ASSERT(iter2 != NULL);
    ASSERT(iter2->value == param2);
    kefir_list_next(&iter2);
    ASSERT(iter2 == NULL);
    ASSERT(func1->body == body1);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(func1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_translation_units1, "AST nodes - translation units #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_translation_unit *unit1 = kefir_ast_new_translation_unit(&kft_mem);
    ASSERT(unit1 != NULL);
    ASSERT(unit1->base.klass->type == KEFIR_AST_TRANSLATION_UNIT);
    ASSERT(unit1->base.self == unit1);
    ASSERT(kefir_list_length(&unit1->external_definitions) == 0);

    struct kefir_ast_declaration_list *decl1 = kefir_ast_new_single_declaration_list(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, &symbols, "xyz"),
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))),
        NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &unit1->external_definitions,
                                      kefir_list_tail(&unit1->external_definitions), KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_declaration_list *decl2 = kefir_ast_new_single_declaration_list(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, &symbols, "abc")),
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))),
        NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2->specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &unit1->external_definitions,
                                      kefir_list_tail(&unit1->external_definitions), KEFIR_AST_NODE_BASE(decl2)));

    struct kefir_ast_declarator *decl3 =
        kefir_ast_declarator_function(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, &symbols, "fn1"));
    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(&kft_mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(&kft_mem, decl3, body1);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &unit1->external_definitions,
                                      kefir_list_tail(&unit1->external_definitions), KEFIR_AST_NODE_BASE(func1)));

    const struct kefir_list_entry *iter = kefir_list_head(&unit1->external_definitions);
    ASSERT(iter != NULL);
    ASSERT(iter->value == KEFIR_AST_NODE_BASE(decl1));
    kefir_list_next(&iter);
    ASSERT(iter != NULL);
    ASSERT(iter->value == KEFIR_AST_NODE_BASE(decl2));
    kefir_list_next(&iter);
    ASSERT(iter != NULL);
    ASSERT(iter->value == KEFIR_AST_NODE_BASE(func1));
    kefir_list_next(&iter);
    ASSERT(iter == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(unit1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_declaration_list1, "AST nodes - declaration list #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_declaration_list *declaration_list = kefir_ast_new_declaration_list(&kft_mem);
    ASSERT(declaration_list != NULL);
    ASSERT(declaration_list->base.klass->type == KEFIR_AST_DECLARATION_LIST);
    ASSERT(declaration_list->base.self == declaration_list);
    ASSERT(kefir_list_length(&declaration_list->declarations) == 0);
    struct kefir_ast_declarator_specifier *specifier = NULL;
    ASSERT(kefir_ast_declarator_specifier_list_iter(&declaration_list->specifiers, &specifier) == NULL);

    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
        &kft_mem, declaration_list, kefir_ast_declarator_identifier(&kft_mem, &symbols, "A"), NULL);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &declaration_list->declarations,
                                      kefir_list_tail(&declaration_list->declarations), KEFIR_AST_NODE_BASE(decl1)));
    struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
        &kft_mem, declaration_list, kefir_ast_declarator_identifier(&kft_mem, &symbols, "B"), NULL);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &declaration_list->declarations,
                                      kefir_list_tail(&declaration_list->declarations), KEFIR_AST_NODE_BASE(decl2)));
    struct kefir_ast_declaration *decl3 = kefir_ast_new_declaration(
        &kft_mem, declaration_list, kefir_ast_declarator_identifier(&kft_mem, &symbols, "B"), NULL);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &declaration_list->declarations,
                                      kefir_list_tail(&declaration_list->declarations), KEFIR_AST_NODE_BASE(decl3)));

    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &declaration_list->specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &declaration_list->specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));

    ASSERT(kefir_list_length(&declaration_list->declarations) == 3);
    const struct kefir_list_entry *iter = kefir_list_head(&declaration_list->declarations);
    ASSERT(iter != NULL);
    ASSERT(iter->value == KEFIR_AST_NODE_BASE(decl1));
    kefir_list_next(&iter);
    ASSERT(iter != NULL);
    ASSERT(iter->value == KEFIR_AST_NODE_BASE(decl2));
    kefir_list_next(&iter);
    ASSERT(iter != NULL);
    ASSERT(iter->value == KEFIR_AST_NODE_BASE(decl3));
    kefir_list_next(&iter);
    ASSERT(iter == NULL);

    struct kefir_list_entry *iter2 =
        kefir_ast_declarator_specifier_list_iter(&declaration_list->specifiers, &specifier);
    ASSERT(iter2 != NULL && specifier != NULL);
    ASSERT(specifier->klass == KEFIR_AST_TYPE_SPECIFIER);
    ASSERT(specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_INT);
    ASSERT_OK(kefir_ast_declarator_specifier_list_next(&iter2, &specifier));
    ASSERT(iter2 != NULL && specifier != NULL);
    ASSERT(specifier->klass == KEFIR_AST_TYPE_QUALIFIER);
    ASSERT(specifier->type_qualifier == KEFIR_AST_TYPE_QUALIFIER_CONST);
    ASSERT_OK(kefir_ast_declarator_specifier_list_next(&iter2, &specifier));
    ASSERT(iter2 == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(declaration_list)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
