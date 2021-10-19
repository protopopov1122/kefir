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
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

struct kefir_ast_constant *make_constant(struct kefir_mem *, const struct kefir_ast_type *);

#define ASSERT_SIMPLE_ASSIGNMENT(_mem, _context, _target, _value, _type)                                           \
    do {                                                                                                           \
        struct kefir_ast_assignment_operator *oper = kefir_ast_new_simple_assignment((_mem), (_target), (_value)); \
        ASSERT(oper != NULL);                                                                                      \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(oper)));                          \
        ASSERT(oper->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);                              \
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, (_type)));                                          \
        ASSERT(!oper->base.properties.expression_props.lvalue);                                                    \
        ASSERT(!oper->base.properties.expression_props.bitfield);                                                  \
        ASSERT(!oper->base.properties.expression_props.addressable);                                               \
        ASSERT(!oper->base.properties.expression_props.constant_expression);                                       \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper)));                                         \
    } while (0)

#define ASSERT_SIMPLE_ASSIGNMENT_NOK(_mem, _context, _target, _value)                                              \
    do {                                                                                                           \
        struct kefir_ast_assignment_operator *oper = kefir_ast_new_simple_assignment((_mem), (_target), (_value)); \
        ASSERT(oper != NULL);                                                                                      \
        ASSERT_NOK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(oper)));                         \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper)));                                         \
    } while (0)

DEFINE_CASE(ast_node_analysis_simple_assignment_operator1, "AST node analysis - simple assignment operator #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type1);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "x", kefir_ast_type_signed_int(),
                                                        NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(
        &kft_mem, &global_context, "y",
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}),
        NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "z", type1, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "X",
                                                      kefir_ast_constant_expression_integer(&kft_mem, 300),
                                                      type_traits->underlying_enumeration_type, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "w", kefir_ast_type_unsigned_char(),
                                                        NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "a", kefir_ast_type_float(), NULL,
                                                        NULL, NULL));

    ASSERT_SIMPLE_ASSIGNMENT(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)), kefir_ast_type_signed_int());

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 101)));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 102)));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 103)));

    ASSERT_SIMPLE_ASSIGNMENT(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "w")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 104)), kefir_ast_type_unsigned_char());

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 105)), kefir_ast_type_float());

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 103)));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_simple_assignment_operator2, "AST node analysis - simple assignment operator #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "x", kefir_ast_type_signed_int(),
                                                        NULL, NULL, NULL));

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_boolean(),          kefir_ast_type_char(),         kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),      kefir_ast_type_unsigned_int(), kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),    kefir_ast_type_signed_long(),  kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(), kefir_ast_type_float(),        kefir_ast_type_double()};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    struct kefir_ast_type_name *TYPES2[] = {
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
    };

    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[0]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_bool(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[1]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[2]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[2]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[3]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[3]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[4]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[4]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[5]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[7]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[8]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[8]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[8]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[9]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[9]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[10]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES2[11]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_double(&kft_mem)));

    _Static_assert(sizeof(TYPES) / sizeof(TYPES[0]) == sizeof(TYPES2) / sizeof(TYPES2[0]),
                   "Type array length mismatch");

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_SIMPLE_ASSIGNMENT(
            &kft_mem, context,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_INDIRECTION,
                KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                    &kft_mem, TYPES2[i], KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))))),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])), TYPES[i]);

        ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
                                 KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])), kefir_ast_type_signed_int());
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_simple_assignment_operator3, "AST node analysis - simple assignment operator #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "a", kefir_ast_type_char(), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "b",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_short(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 10), NULL),
                                    NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "c",
                                          kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
                                          NULL));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type2);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type2, "x", kefir_ast_type_unsigned_long(),
                                          NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type2, "y",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 12), NULL),
                                    NULL));
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type2, "z",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()), NULL));

    struct kefir_ast_struct_type *struct_type3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type3);
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "j", kefir_ast_type_double(), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "k",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, type2,
                                                         kefir_ast_constant_expression_integer(&kft_mem, 3), NULL),
                                    NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "l",
                                          kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1), NULL));

    struct kefir_ast_struct_type *union_type4 = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_union(&kft_mem, context->type_bundle, "", &union_type4);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type4, "j", kefir_ast_type_double(), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type4, "k",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, type2,
                                                         kefir_ast_constant_expression_integer(&kft_mem, 3), NULL),
                                    NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type4, "l",
                                          kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1), NULL));

    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "x1", type1, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "x2", type1, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "y1", type2, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "y2", type2, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "z1", type3, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "z2", type3, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "w1", type4, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "w2", type4, NULL, NULL, NULL, NULL));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x1")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x2")), type1);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x2")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x1")), type1);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x1")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y2")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y1")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y2")), type2);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y2")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y1")), type2);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y1")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z1")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z1")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z2")), type3);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z2")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z1")), type3);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z2")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x1")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z1")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "w1")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "w1")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "w2")), type4);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "w2")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "w1")), type4);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "w1")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y2")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "w2")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z1")));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_simple_assignment_operator4, "AST node analysis - simple assignment operator #4") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_int());

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}));

    const struct kefir_ast_type *type3 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = false, .restricted = false, .volatile_type = true}));

    const struct kefir_ast_type *type4 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = true}));

    const struct kefir_ast_type *type5 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = false}));

    const struct kefir_ast_type *type6 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = true}));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "a", type1, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "b", type2, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "c", type3, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "d", type4, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "e", type5, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "f", type6, NULL, NULL, NULL));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), type1);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), type2);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), type3);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), type4);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), type5);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), type6);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")), type2);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")), type4);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")), type5);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")), type6);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")), type3);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")), type4);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")), type6);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")), type4);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")), type6);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")), type5);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")), type6);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "e")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "f")), type6);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_simple_assignment_operator5, "AST node analysis - simple assignment operator #5") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_int());

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}));

    const struct kefir_ast_type *type3 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = true}));

    const struct kefir_ast_type *type4 = kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void());

    const struct kefir_ast_type *type5 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_void(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}));

    const struct kefir_ast_type *type6 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_void(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = true}));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "a", type1, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "b", type2, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "c", type3, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "x", type4, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "y", type5, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "z", type6, NULL, NULL, NULL));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")), type1);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), type4);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), type5);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), type6);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")), type2);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")), type2);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")), type5);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")), type6);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")), type3);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")), type3);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")), type3);

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")));

    ASSERT_SIMPLE_ASSIGNMENT_NOK(&kft_mem, context,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")), type6);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_simple_assignment_operator6, "AST node analysis - simple assignment operator #6") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_int());

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}));

    const struct kefir_ast_type *type3 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = true}));

    const struct kefir_ast_type *type4 = kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void());

    const struct kefir_ast_type *type5 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_void(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}));

    const struct kefir_ast_type *type6 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_void(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = true}));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "a", type1, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "b", type2, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "c", type3, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "x", type4, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "y", type5, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "z", type6, NULL, NULL, NULL));

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), type1);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), type2);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), type3);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), type4);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), type5);

    ASSERT_SIMPLE_ASSIGNMENT(&kft_mem, context,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), type6);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_simple_assignment_operator7, "AST node analysis - simple assignment operator #7") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_int());

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_void(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = true}));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "a", type1, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "b", type2, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "x", kefir_ast_type_boolean(), NULL,
                                                       NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(
        &kft_mem, &local_context, "y",
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_boolean(),
            (struct kefir_ast_type_qualification){.constant = false, .restricted = true, .volatile_type = true}),
        NULL, NULL, NULL));

    ASSERT_SIMPLE_ASSIGNMENT(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), kefir_ast_type_boolean());

    ASSERT_SIMPLE_ASSIGNMENT(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")), kefir_ast_type_boolean());

    ASSERT_SIMPLE_ASSIGNMENT(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a")), kefir_ast_type_boolean());

    ASSERT_SIMPLE_ASSIGNMENT(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b")), kefir_ast_type_boolean());

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

// TODO: Implement compound assignment tests
