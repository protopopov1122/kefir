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
#include "kefir/ast/initializer.h"
#include "kefir/ast/analyzer/initializer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/node.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_initializer_analysis1, "AST initializer - analysis #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_boolean(),
        kefir_ast_type_char(),
        kefir_ast_type_signed_char(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_short(),
        kefir_ast_type_unsigned_short(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double(),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                               kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_int()))};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)));
    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);

    struct kefir_ast_initializer_properties traversal_props;
    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, TYPES[i], init1, &traversal_props));
        ASSERT(traversal_props.constant);
        ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type, TYPES[i]));
        ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, TYPES[i], init2, &traversal_props));
        ASSERT(traversal_props.constant);
        ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type, TYPES[i]));
        ASSERT_NOK(kefir_ast_analyze_initializer(&kft_mem, context, TYPES[i], init3, NULL));
    }
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_analysis2, "AST initializer - analysis #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *structure1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &structure1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1, "x",
                                          kefir_ast_type_signed_long_long(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1, "y",
                                          kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()),
                                          NULL));

    struct kefir_ast_initializer_properties traversal_props;
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init1, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type, type1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init2, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type, type1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 11)))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 110000)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init3, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type, type1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    struct kefir_ast_initializer *init4 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 11)))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3.14)))));
    ASSERT_NOK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init4, NULL));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_analysis3, "AST initializer - analysis #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *structure1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &structure1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1, "x",
                                          kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
                                          NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1, "y",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_int(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 5), NULL),
                                    NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1, "z", kefir_ast_type_boolean(), NULL));

    struct kefir_ast_struct_type *union2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_union(&kft_mem, context->type_bundle, "", &union2);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union2, "f1", type1, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union2, "f2", kefir_ast_type_float(), NULL));

    struct kefir_ast_struct_type *structure3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &structure3);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure3, "a", type2, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure3, "b",
                                          kefir_ast_type_pointer(&kft_mem, context->type_bundle, type2), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, structure3, "c",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_char(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 2), NULL),
                                    NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, structure3, "d",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_char(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 100), NULL),
                                    NULL));

    struct kefir_ast_initializer_properties traversal_props;
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type3, init1, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type, type3));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list, kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "f2", NULL),
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 10.0f)))));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));

    struct kefir_ast_initializer *init4 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));

    struct kefir_ast_initializer *init5 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")));

    struct kefir_ast_initializer *init_final = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init_final->list, NULL, init2));

    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init_final->list,
        kefir_ast_new_initializer_member_designation(
            &kft_mem, context->symbols, "x",
            kefir_ast_new_initializer_member_designation(
                &kft_mem, context->symbols, "f1",
                kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "a", NULL))),
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));

    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init_final->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)))));

    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init_final->list,
        kefir_ast_new_initializer_index_designation(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4)),
            kefir_ast_new_initializer_member_designation(
                &kft_mem, context->symbols, "y",
                kefir_ast_new_initializer_member_designation(
                    &kft_mem, context->symbols, "f1",
                    kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "a", NULL)))),
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4)))));

    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init_final->list,
        kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "c", NULL), init3));

    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init_final->list,
        kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "d", NULL), init4));

    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init_final->list,
        kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "d", NULL), init5));

    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type3, init_final, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type, type3));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init_final));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_analysis4, "AST initializer - analysis #4") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 = kefir_ast_type_unbounded_array(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_char(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}),
        NULL);

    struct kefir_ast_initializer_properties traversal_props;
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init1, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 0), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init2, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 14), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init3, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 14), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    struct kefir_ast_initializer *init4 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init4, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 14), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));

    struct kefir_ast_initializer *init5_1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init5_1->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
    struct kefir_ast_initializer *init5 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init5->list, NULL, init5_1));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init5, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 14), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init5));

    struct kefir_ast_initializer *init6 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'H')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'e')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list,
        kefir_ast_new_initializer_index_designation(&kft_mem,
                                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), NULL),
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init6, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 3), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init6));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_analysis5, "AST initializer - analysis #5") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 = kefir_ast_type_array(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_char(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}),
        kefir_ast_constant_expression_integer(&kft_mem, 10), NULL);

    const struct kefir_ast_type *type2 = kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, type1, NULL);

    struct kefir_ast_initializer_properties traversal_props;
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type2, init1, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 0), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")));
    ASSERT_NOK(kefir_ast_analyze_initializer(&kft_mem, context, type2, init2, &traversal_props));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type2, init3, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 1), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    struct kefir_ast_initializer *init4 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world2!")))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type2, init4, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 2), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));

    struct kefir_ast_initializer *init5 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init5->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init5->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type2, init5, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 2), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init5));

    struct kefir_ast_initializer *init6_1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6_1->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
    struct kefir_ast_initializer *init6 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init6->list, NULL, init6_1));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type2, init6, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 1), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init6));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_analysis_unicode8, "AST initializer - unicode8 string analysis") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_char(), NULL);

    struct kefir_ast_initializer_properties traversal_props;
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init1, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 0), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    const char LITERAL1[] = "Hello, world!";
    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode8(&kft_mem, LITERAL1, sizeof(LITERAL1))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init2, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode8(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init3, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    struct kefir_ast_initializer *init4 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode8(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 200)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init4, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));

    struct kefir_ast_initializer *init5_1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init5_1->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode8(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    struct kefir_ast_initializer *init5 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init5->list, NULL, init5_1));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init5, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init5));

    struct kefir_ast_initializer *init6 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, u'H')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, u'e')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, u'l')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list,
        kefir_ast_new_initializer_index_designation(&kft_mem,
                                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), NULL),
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, u'l')))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init6, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 3), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init6));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_analysis_unicode16, "AST initializer - unicode16 string analysis") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_short(), NULL);

    struct kefir_ast_initializer_properties traversal_props;
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init1, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 0), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    const kefir_char16_t LITERAL1[] = u"Hello, world!";
    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode16(&kft_mem, LITERAL1, sizeof(LITERAL1))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init2, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode16(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init3, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    struct kefir_ast_initializer *init4 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode16(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 200)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init4, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));

    struct kefir_ast_initializer *init5_1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init5_1->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode16(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    struct kefir_ast_initializer *init5 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init5->list, NULL, init5_1));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init5, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init5));

    struct kefir_ast_initializer *init6 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode16_char(&kft_mem, u'H')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode16_char(&kft_mem, u'e')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode16_char(&kft_mem, u'l')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list,
        kefir_ast_new_initializer_index_designation(&kft_mem,
                                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), NULL),
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode16_char(&kft_mem, u'l')))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init6, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 3), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init6));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_analysis_unicode32, "AST initializer - unicode32 string analysis") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_int(), NULL);

    struct kefir_ast_initializer_properties traversal_props;
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init1, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 0), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    const kefir_char32_t LITERAL1[] = U"Hello, world!";
    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode32(&kft_mem, LITERAL1, sizeof(LITERAL1))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init2, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode32(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init3, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    struct kefir_ast_initializer *init4 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode32(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 200)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init4, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));

    struct kefir_ast_initializer *init5_1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init5_1->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode32(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    struct kefir_ast_initializer *init5 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init5->list, NULL, init5_1));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init5, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init5));

    struct kefir_ast_initializer *init6 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode32_char(&kft_mem, U'H')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode32_char(&kft_mem, U'e')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode32_char(&kft_mem, U'l')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list,
        kefir_ast_new_initializer_index_designation(&kft_mem,
                                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), NULL),
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode32_char(&kft_mem, U'l')))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init6, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 3), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init6));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_analysis_wide, "AST initializer - wide string analysis") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, type_traits->wide_char_type, NULL);

    struct kefir_ast_initializer_properties traversal_props;
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init1, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 0), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    const kefir_wchar_t LITERAL1[] = L"Hello, world!";
    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_wide(&kft_mem, LITERAL1, sizeof(LITERAL1))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init2, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init3->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_wide(&kft_mem, LITERAL1, sizeof(LITERAL1))))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init3, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    struct kefir_ast_initializer *init4 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_wide(&kft_mem, LITERAL1, sizeof(LITERAL1))))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init4->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 200)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init4, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));

    struct kefir_ast_initializer *init5_1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init5_1->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_wide(&kft_mem, LITERAL1, sizeof(LITERAL1))))));
    struct kefir_ast_initializer *init5 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init5->list, NULL, init5_1));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init5, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(
        traversal_props.type,
        kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                             kefir_ast_constant_expression_integer(&kft_mem, sizeof(LITERAL1)), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init5));

    struct kefir_ast_initializer *init6 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_wide_char(&kft_mem, U'H')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_wide_char(&kft_mem, U'e')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_wide_char(&kft_mem, U'l')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init6->list,
        kefir_ast_new_initializer_index_designation(&kft_mem,
                                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), NULL),
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_wide_char(&kft_mem, U'l')))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init6, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, type1->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(&kft_mem, 3), NULL)));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init6));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_analysis_mixed_strings, "AST initializer - mixed string analysis") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &struct_type1);
    REQUIRE_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "narrow",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 16), NULL),
                                    NULL));
    REQUIRE_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "unicode8",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 16), NULL),
                                    NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type1, "unicode16",
        kefir_ast_type_array(&kft_mem, context->type_bundle, type_traits->unicode16_char_type,
                             kefir_ast_constant_expression_integer(&kft_mem, 16), NULL),
        NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type1, "unicode32",
        kefir_ast_type_array(&kft_mem, context->type_bundle, type_traits->unicode32_char_type,
                             kefir_ast_constant_expression_integer(&kft_mem, 16), NULL),
        NULL));
    REQUIRE_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "wide",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, type_traits->wide_char_type,
                                                         kefir_ast_constant_expression_integer(&kft_mem, 16), NULL),
                                    NULL));

    struct kefir_ast_initializer_properties traversal_props;
    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init1, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type, type1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    const char LITERAL1[] = "Hello, world!";
    const char LITERAL2[] = u8"Goodbye, world!";
    const kefir_char16_t LITERAL3[] = u"It's me. Again";
    const kefir_char32_t LITERAL4[] = U"One more time";
    const kefir_wchar_t LITERAL5[] = L"The real goodbye";
    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_multibyte(
                                                           &kft_mem, LITERAL1, sizeof(LITERAL1))))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode8(
                                                           &kft_mem, LITERAL2, sizeof(LITERAL2))))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode16(
                                                           &kft_mem, LITERAL3, sizeof(LITERAL3))))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_unicode32(
                                                           &kft_mem, LITERAL4, sizeof(LITERAL4))))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal_wide(&kft_mem, LITERAL5, sizeof(LITERAL5))))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list,
        kefir_ast_new_initializer_index_designation(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
            kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "narrow", NULL)),
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list,
        kefir_ast_new_initializer_index_designation(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 3)),
            kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "wide", NULL)),
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_wide_char(&kft_mem, U'b')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list,
        kefir_ast_new_initializer_index_designation(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
            kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "unicode16", NULL)),
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode16_char(&kft_mem, u'c')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list,
        kefir_ast_new_initializer_index_designation(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4)),
            kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "unicode32", NULL)),
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode32_char(&kft_mem, U'd')))));
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init2->list,
        kefir_ast_new_initializer_index_designation(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)),
            kefir_ast_new_initializer_member_designation(&kft_mem, context->symbols, "unicode8", NULL)),
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'e')))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init2, &traversal_props));
    ASSERT(traversal_props.constant);
    ASSERT(KEFIR_AST_TYPE_SAME(traversal_props.type, type1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
