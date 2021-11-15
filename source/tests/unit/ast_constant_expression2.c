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
#include "kefir/ast/constant_expression.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/test/util.h"
#include "constant_expression.h"

DEFINE_CASE(ast_constant_expression_cast_operator1, "AST constant expressions - cast operator #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_type_name *TYPES[] = {
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[0]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_boolean(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[1]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[2]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[2]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[3]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[3]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[4]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[4]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_short(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[5]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_short(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[7]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[8]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[8]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[9]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[10]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[10]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[10]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[11]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[11]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[12]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[13]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_double(&kft_mem)));

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_int_t j = -10; j < 10; j++) {
            struct kefir_ast_type_name *type_name =
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(TYPES[i]));
            ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(type_name)));
            if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type_name->base.properties.type)) {
                ASSERT_INTEGER_CONST_EXPR(
                    &kft_mem, context,
                    kefir_ast_new_cast_operator(&kft_mem, type_name,
                                                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, j))),
                    j);
            } else {
                ASSERT_FLOAT_CONST_EXPR(
                    &kft_mem, context,
                    kefir_ast_new_cast_operator(&kft_mem, type_name,
                                                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, j))),
                    (double) j);
            }
        }
    }

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_float64_t f = -10.0f; f < 10.0f; f += 0.1f) {
            struct kefir_ast_type_name *type_name =
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(TYPES[i]));
            ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(type_name)));
            if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type_name->base.properties.type)) {
                ASSERT_INTEGER_CONST_EXPR(
                    &kft_mem, context,
                    kefir_ast_new_cast_operator(&kft_mem, type_name,
                                                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
                    (kefir_int64_t) f);
            } else {
                ASSERT_FLOAT_CONST_EXPR(
                    &kft_mem, context,
                    kefir_ast_new_cast_operator(&kft_mem, type_name,
                                                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
                    f);
            }
        }
    }

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(TYPES[i])));
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_constant_expression_cast_operator2, "AST constant expressions - cast operator #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    for (kefir_size_t i = 0; i < 100; i++) {
        struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
        ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                             kefir_ast_type_specifier_void(&kft_mem)));
        ASSERT_INTPTR_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_cast_operator(&kft_mem, type_name1,
                                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            i, 0);

        struct kefir_ast_type_name *type_name2 = kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
        ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                             kefir_ast_type_specifier_long(&kft_mem)));
        ASSERT_INTPTR_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_cast_operator(&kft_mem, type_name2,
                                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            i, 0);

        struct kefir_ast_type_name *type_name3 = kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
        ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name3->type_decl.specifiers,
                                                             kefir_ast_type_specifier_double(&kft_mem)));
        ASSERT_INTPTR_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_cast_operator(&kft_mem, type_name3,
                                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            i, 0);

        struct kefir_ast_type_name *type_name4 = kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(
                          &kft_mem, kefir_ast_declarator_pointer(
                                        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))));
        ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name4->type_decl.specifiers,
                                                             kefir_ast_type_specifier_int(&kft_mem)));
        ASSERT_INTPTR_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_cast_operator(&kft_mem, type_name4,
                                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            i, 0);
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_constant_expression_conditional_operator1, "AST constant expressions - conditional operators #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))),
        1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 3))),
        3);

    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3.14)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 25))),
        3.14);

    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 2.71))),
        2.71);

    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 100.5)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, -1))),
        100.5);

    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, -27.5)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(&kft_mem, 19.01))),
        19.01);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -1)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -2))),
        -1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 0.0)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 16)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 14))),
        14);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_constant_expression_conditional_operator2, "AST constant expressions - conditional operators #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "variableX",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(
            &kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "variableX")))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))),
        1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 200))),
        10);

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(
            &kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                &kft_mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 7))),
        7);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(
            &kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                &kft_mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 6)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 9))),
        6);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(
            &kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADD,
                KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                    &kft_mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 101)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 202))),
        101);

    struct kefir_ast_type_name *type_name2 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_conditional_operator(
            &kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                &kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                    &kft_mem, type_name2, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4)))),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 101)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 202))),
        202);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
