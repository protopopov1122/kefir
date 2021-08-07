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
    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "character1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'C'))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem, kefir_ast_declarator_function(
                     mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_character1"))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "character1"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit2(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "integer1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0xfefea6))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl2)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem,
            kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_integer1"))),
        body1);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "integer1"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit3(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_declaration *decl3 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "long1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, -1234543))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl3)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(mem, kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(
                                                                                 mem, context->symbols, "get_long1"))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "long1"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit4(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_declaration *decl4 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "float1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 9.57463f))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl4->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl4->specifiers, kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl4)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(mem, kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(
                                                                                 mem, context->symbols, "get_float1"))),
        body1);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "float1"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit5(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_declaration *decl5 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "double1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1.564e14))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl5->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl5->specifiers, kefir_ast_type_specifier_double(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl5)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem,
            kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_double1"))),
        body1);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_double(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "double1"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit6(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_declaration *decl6 = kefir_ast_new_declaration(
        mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                   kefir_ast_declarator_identifier(mem, context->symbols, "str1")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "Ping-pong-ping-pong"))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl6->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl6->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl6)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(mem, kefir_ast_declarator_function(
                                              mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_str1"))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "str1"))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit7(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_declaration *decl7 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "str2")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "   ....\t\t\t\\\n\n\n...TEST\n "))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl7->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl7->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl7)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem, kefir_ast_declarator_pointer(
                     mem, kefir_ast_declarator_function(
                              mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_str2")))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "str2"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit8(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_declaration *decl8 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "str3")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "str1"))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl8->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl8->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl8)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem, kefir_ast_declarator_pointer(
                     mem, kefir_ast_declarator_function(
                              mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_str3")))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "str3"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit9(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   struct kefir_list *unit) {
    struct kefir_ast_declaration *decl9 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "int1ptr")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "integer1"))))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl9->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl9->specifiers, kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl9)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem, kefir_ast_declarator_pointer(
                     mem, kefir_ast_declarator_function(
                              mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_int1ptr")))),
        body1);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int1ptr"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit10(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                    struct kefir_list *unit) {
    struct kefir_ast_declaration *decl10 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "fnptr")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "get_integer1"))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl10->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl10->specifiers, kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl10)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(mem, kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(
                                                                                 mem, context->symbols, "get_fnptr"))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "fnptr"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
    return KEFIR_OK;
}

static kefir_result_t define_unit11(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                    struct kefir_list *unit) {
    struct kefir_ast_declaration *decl11 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "null_ptr")),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl11->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl11->specifiers, kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(decl11)));

    struct kefir_ast_compound_statement *body1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_function_definition *func1 = kefir_ast_new_function_definition(
        mem,
        kefir_ast_declarator_pointer(
            mem, kefir_ast_declarator_pointer(
                     mem, kefir_ast_declarator_function(
                              mem, kefir_ast_declarator_identifier(mem, context->symbols, "get_null_ptr")))),
        body1);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &func1->specifiers, kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &body1->block_items, kefir_list_tail(&body1->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "null_ptr"))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, unit, kefir_list_tail(unit), KEFIR_AST_NODE_BASE(func1)));
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
    REQUIRE_OK(define_unit7(mem, context, unit));
    REQUIRE_OK(define_unit8(mem, context, unit));
    REQUIRE_OK(define_unit9(mem, context, unit));
    REQUIRE_OK(define_unit10(mem, context, unit));
    REQUIRE_OK(define_unit11(mem, context, unit));
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
