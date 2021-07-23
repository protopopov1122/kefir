/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

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

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    struct kefir_ast_local_context local_context2;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context2));
    const struct kefir_ast_context *context = &local_context.context;

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_enum_specifier *specifier1 = kefir_ast_enum_specifier_init(mem, context->symbols, "enum1", true);
    REQUIRE_OK(kefir_ast_enum_specifier_append(mem, specifier1, context->symbols, "A", NULL));
    REQUIRE_OK(kefir_ast_enum_specifier_append(mem, specifier1, context->symbols, "B", NULL));
    REQUIRE_OK(kefir_ast_enum_specifier_append(mem, specifier1, context->symbols, "C", NULL));
    struct kefir_ast_type_name *type_name_A = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name_A->type_decl.specifiers,
                                                          kefir_ast_type_specifier_enum(mem, specifier1)));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(type_name_A)));
    const struct kefir_ast_type *type_A = type_name_A->base.properties.type->referenced_type;

    struct kefir_ast_type_name *type_name_B = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_array(
                 mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 16)),
                 kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name_B->type_decl.specifiers,
                                                          kefir_ast_type_specifier_short(mem)));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(type_name_B)));
    const struct kefir_ast_type *type_B = type_name_B->base.properties.type->referenced_type;

    struct kefir_ast_structure_specifier *specifier2 = kefir_ast_structure_specifier_init(mem, NULL, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context->symbols, "field1"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier2, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry2->declaration.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry2, kefir_ast_declarator_identifier(mem, context->symbols, "field2"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier2, entry2));

    struct kefir_ast_type_name *type_name_C = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &type_name_C->type_decl.specifiers,
        kefir_ast_type_specifier_struct(mem, kefir_ast_structure_specifier_clone(mem, specifier2))));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(type_name_C)));
    const struct kefir_ast_type *type_C = type_name_C->base.properties.type->referenced_type;

    struct kefir_ast_type_name *type_name_D = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &type_name_D->type_decl.specifiers,
        kefir_ast_type_specifier_union(mem, kefir_ast_structure_specifier_clone(mem, specifier2))));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(type_name_D)));
    const struct kefir_ast_type *type_D = type_name_D->base.properties.type->referenced_type;

    REQUIRE_OK(kefir_ast_global_context_define_external(
        mem, &global_context, "X",
        kefir_ast_type_qualified(mem, context->type_bundle, type_A,
                                 (struct kefir_ast_type_qualification){.constant = true}),
        NULL, NULL, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context, "Y", type_B, NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context, "Z", type_C, NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context, "W", type_D, NULL, NULL, NULL));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(
        kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
        mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC("plus", {
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100)));
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 100)));
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 3.14f)));
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 3.14159)));
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
    });

    FUNC("minus", {
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -512)));
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 128)));
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2.71f)));
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 2.71828)));
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
    });

    FUNC("invert", {
        UNARY_NODE(KEFIR_AST_OPERATION_INVERT, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A')));
        UNARY_NODE(KEFIR_AST_OPERATION_INVERT, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, -100500)));
        UNARY_NODE(KEFIR_AST_OPERATION_INVERT, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 45)));
        UNARY_NODE(KEFIR_AST_OPERATION_INVERT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
    });

    FUNC("logical_negate", {
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1)));
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 0)));
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1.76f)));
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1.23456)));
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
    });

    struct kefir_ast_type_name *TYPES[] = {
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem,
                                kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL))),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem,
                                kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 16)),
                                                           kefir_ast_declarator_identifier(mem, NULL, NULL))),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL))};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[0]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_bool(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[1]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[2]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[2]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[3]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_signed(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[3]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[4]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[4]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_short(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[5]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_short(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[6]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[6]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[7]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[8]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[8]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[9]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[10]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[10]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[10]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[11]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[11]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[12]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[13]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_double(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[14]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &TYPES[15]->type_decl.specifiers,
        kefir_ast_type_specifier_enum(mem, kefir_ast_enum_specifier_init(mem, context->symbols, "enum1", false))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[16]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_short(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(
        mem, &TYPES[17]->type_decl.specifiers,
        kefir_ast_type_specifier_struct(mem, kefir_ast_structure_specifier_clone(mem, specifier2))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[18]->type_decl.specifiers,
                                                          kefir_ast_type_specifier_union(mem, specifier2)));

    FUNC("sizeof1", {
        for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
            UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(TYPES[i])));
        }
    });

    FUNC("sizeof2", {
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A')));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 100)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 100200)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 110200)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 110)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(mem, 5110)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 5.67f)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 8562.8483)));

        struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
            mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers,
                                                              kefir_ast_type_specifier_void(mem)));

        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                       mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "Y")));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "Z")));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "W")));
    });

    context = &local_context2.context;
    FUNC("alignof", {
        for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
            UNARY_NODE(KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(TYPES[i]));
        }
    });

    FUNC("indirection", {
        struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
            mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers,
                                                              kefir_ast_type_specifier_char(mem)));

        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                       mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))));

        struct kefir_ast_type_name *type_name2 = kefir_ast_new_type_name(
            mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name2->type_decl.specifiers,
                                                              kefir_ast_type_specifier_short(mem)));

        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                       mem, type_name2, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))));

        struct kefir_ast_type_name *type_name3 = kefir_ast_new_type_name(
            mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name3->type_decl.specifiers,
                                                              kefir_ast_type_specifier_float(mem)));

        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                       mem, type_name3, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)))));

        struct kefir_ast_type_name *type_name4 = kefir_ast_new_type_name(
            mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name4->type_decl.specifiers,
                                                              kefir_ast_type_specifier_long(mem)));

        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                       mem, type_name4, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)))));

        struct kefir_ast_type_name *type_name5 = kefir_ast_new_type_name(
            mem, kefir_ast_declarator_pointer(
                     mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL))));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name5->type_decl.specifiers,
                                                              kefir_ast_type_specifier_bool(mem)));

        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                       mem, KEFIR_AST_OPERATION_INDIRECTION,
                       KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                           mem, type_name5, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)))))));

        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                       mem, type_name_A, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                       mem, type_name_B, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 6)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                       mem, type_name_C, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 7)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                       mem, type_name_D, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 8)))));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context2));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
