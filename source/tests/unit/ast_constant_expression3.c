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
#include "declarator_analysis.h"

DEFINE_CASE(ast_constant_expression_unary_operations1, "AST constant expressions - unary operations #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    for (kefir_int64_t i = -100; i < 100; i++) {
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_PLUS,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            +i);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_NEGATE,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            -i);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_INVERT,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            ~i);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            !i);
    }

    for (kefir_float64_t f = -100.0f; f < 100.0f; f += 0.01f) {
        ASSERT_FLOAT_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_PLUS,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
            +f);
        ASSERT_FLOAT_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_NEGATE,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
            -f);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
            !f);
        ASSERT_FLOAT_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_PLUS,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(&kft_mem, f))),
            +f);
        ASSERT_FLOAT_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_NEGATE,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(&kft_mem, f))),
            -f);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(&kft_mem, f))),
            !f);
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations2, "AST constant expressions - unary operations #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

#define MAKE_TYPENAME(_id, _spec_count, ...)                                                      \
    struct kefir_ast_type_name *_id =                                                             \
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)); \
    ASSERT_OK(append_specifiers(&kft_mem, &_id->type_decl.specifiers, (_spec_count), __VA_ARGS__));

    MAKE_TYPENAME(type_name1, 1, kefir_ast_type_specifier_boolean(&kft_mem));
    MAKE_TYPENAME(type_name2, 1, kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name3, 2, kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name4, 2, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name5, 1, kefir_ast_type_specifier_short(&kft_mem));
    MAKE_TYPENAME(type_name6, 2, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_short(&kft_mem));
    MAKE_TYPENAME(type_name7, 1, kefir_ast_type_specifier_signed(&kft_mem));
    MAKE_TYPENAME(type_name8, 1, kefir_ast_type_specifier_unsigned(&kft_mem));
    MAKE_TYPENAME(type_name9, 1, kefir_ast_type_specifier_long(&kft_mem));
    MAKE_TYPENAME(type_name10, 2, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));
    MAKE_TYPENAME(type_name11, 2, kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));
    MAKE_TYPENAME(type_name12, 3, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                  kefir_ast_type_specifier_long(&kft_mem));
    MAKE_TYPENAME(type_name13, 1, kefir_ast_type_specifier_float(&kft_mem));
    MAKE_TYPENAME(type_name14, 1, kefir_ast_type_specifier_double(&kft_mem));
#undef MAKE_TYPENAME

    struct kefir_ast_type_name *type_name15 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(
        append_specifiers(&kft_mem, &type_name15->type_decl.specifiers, 1, kefir_ast_type_specifier_void(&kft_mem)));

    struct kefir_ast_type_name *type_name16 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 8)),
                                             kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(
        append_specifiers(&kft_mem, &type_name16->type_decl.specifiers, 1, kefir_ast_type_specifier_char(&kft_mem)));

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name1)), 1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name2)), 1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name3)), 1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name4)), 1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name5)), 2);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name6)), 2);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name7)), 4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name8)), 4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name9)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name10)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name11)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name12)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name13)), 4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name14)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name15)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(type_name16)), 8);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations3, "AST constant expressions - unary operations #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true))),
        1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))),
        4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_wide_char(&kft_mem, U'b'))),
        4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode16_char(&kft_mem, U'c'))),
        2);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode32_char(&kft_mem, U'd'))),
        4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -100))),
        4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 200))),
        4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, -100500))),
        8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(&kft_mem, 100500))),
        8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(&kft_mem, -100500))),
        8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(&kft_mem, 100500))),
        8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71828f))),
        4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3.14159))),
        8);

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                             kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));

    struct kefir_ast_compound_literal *compound1 = kefir_ast_new_compound_literal(&kft_mem, type_name1);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &compound1->initializer->list,
        kefir_ast_new_initializer_index_designation(&kft_mem,
                                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 9)), NULL),
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'H')))));

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(compound1)), 40);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations4, "AST constant expressions - unary operations #4") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

#define MAKE_TYPENAME(_id, _spec_count, ...)                                                      \
    struct kefir_ast_type_name *_id =                                                             \
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)); \
    ASSERT_OK(append_specifiers(&kft_mem, &_id->type_decl.specifiers, (_spec_count), __VA_ARGS__));

    MAKE_TYPENAME(type_name1, 1, kefir_ast_type_specifier_boolean(&kft_mem));
    MAKE_TYPENAME(type_name2, 1, kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name3, 2, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name4, 2, kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name5, 2, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_short(&kft_mem));
    MAKE_TYPENAME(type_name6, 1, kefir_ast_type_specifier_short(&kft_mem));
    MAKE_TYPENAME(type_name7, 1, kefir_ast_type_specifier_unsigned(&kft_mem));
    MAKE_TYPENAME(type_name8, 1, kefir_ast_type_specifier_signed(&kft_mem));
    MAKE_TYPENAME(type_name9, 2, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));
    MAKE_TYPENAME(type_name10, 1, kefir_ast_type_specifier_long(&kft_mem));
    MAKE_TYPENAME(type_name11, 3, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                  kefir_ast_type_specifier_long(&kft_mem));
    MAKE_TYPENAME(type_name12, 2, kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));
    MAKE_TYPENAME(type_name13, 1, kefir_ast_type_specifier_float(&kft_mem));
    MAKE_TYPENAME(type_name14, 1, kefir_ast_type_specifier_double(&kft_mem));
#undef MAKE_TYPENAME

    struct kefir_ast_type_name *type_name15 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(
        append_specifiers(&kft_mem, &type_name15->type_decl.specifiers, 1, kefir_ast_type_specifier_short(&kft_mem)));

    struct kefir_ast_type_name *type_name16 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5)),
                                             kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(
        append_specifiers(&kft_mem, &type_name16->type_decl.specifiers, 1, kefir_ast_type_specifier_signed(&kft_mem)));

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name1)), 1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name2)), 1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name3)), 1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name4)), 1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name5)), 2);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name6)), 2);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name7)), 4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name8)), 4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name9)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name10)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name11)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name12)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name13)), 4);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name14)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name15)), 8);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name16)), 4);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations5, "AST constant expressions - unary operations #5") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "fieldX", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct1, "fieldY", kefir_ast_type_signed_int(),
        kefir_ast_alignment_const_expression(&kft_mem, kefir_ast_constant_expression_integer(&kft_mem, 8))));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "fieldZ",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_long(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 8), NULL),
                                    NULL));

    ASSERT_OK(kefir_ast_global_context_declare_external(
        &kft_mem, &global_context, "var1",
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                 (struct kefir_ast_type_qualification){.constant = true}),
        NULL, NULL, NULL));

    ASSERT_OK(kefir_ast_global_context_declare_external(
        &kft_mem, &global_context, "var2",
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, type1,
                                 (struct kefir_ast_type_qualification){.constant = true}),
        NULL, NULL, NULL));

    ASSERT_IDENTIFIER_CONST_EXPR(&kft_mem, context,
                                 kefir_ast_new_unary_operation(
                                     &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"))),
                                 "var1", 0);

    ASSERT_IDENTIFIER_CONST_EXPR(&kft_mem, context,
                                 kefir_ast_new_unary_operation(
                                     &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2"))),
                                 "var2", 0);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(
                &kft_mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")), "fieldX"))),
        "var2", 0);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(
                &kft_mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")), "fieldY"))),
        "var2", 8);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(
                &kft_mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")), "fieldZ"))),
        "var2", 16);

    for (kefir_size_t i = 0; i < 8; i++) {
        ASSERT_IDENTIFIER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
                    &kft_mem,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(
                        &kft_mem, context->symbols,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")), "fieldZ")),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))))),
            "var2", (kefir_int64_t) (16 + i * 8));

        ASSERT_IDENTIFIER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(
                        &kft_mem, context->symbols,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")),
                        "fieldZ"))))),
            "var2", (kefir_int64_t) (16 + i * 8));
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations6, "AST constant expressions - unary operations #6") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "fieldX", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct1, "fieldY", kefir_ast_type_signed_int(),
        kefir_ast_alignment_const_expression(&kft_mem, kefir_ast_constant_expression_integer(&kft_mem, 8))));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "fieldZ",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_long(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 8), NULL),
                                    NULL));

    ASSERT_OK(kefir_ast_global_context_declare_external(
        &kft_mem, &global_context, "var1",
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, type1,
                                 (struct kefir_ast_type_qualification){.constant = true}),
        NULL, NULL, NULL));

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
                &kft_mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                    &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))),
                "fieldX"))),
        "var1", 0);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
                &kft_mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                    &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))),
                "fieldY"))),
        "var1", 8);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
                &kft_mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                    &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))),
                "fieldZ"))),
        "var1", 16);

    for (kefir_size_t i = 0; i < 8; i++) {
        ASSERT_IDENTIFIER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
                    &kft_mem,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
                        &kft_mem, context->symbols,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))),
                        "fieldZ")),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))))),
            "var1", (kefir_int64_t) (16 + i * 8));

        ASSERT_IDENTIFIER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
                        &kft_mem, context->symbols,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))),
                        "fieldZ"))))),
            "var1", (kefir_int64_t) (16 + i * 8));
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations7, "AST constant expressions - unary operations #7") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "variableX",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                              kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                                                            KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(
                                                                &kft_mem, "Hello, world!"))),
                              0);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "variableX"))))),
        0);

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                &kft_mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))))),
        1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                &kft_mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))))),
        0);

    struct kefir_ast_type_name *type_name2 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                &kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                    &kft_mem,
                    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4)))),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))))),
        1);

    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADD,
                KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                    &kft_mem, type_name2, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))))),
        0);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
