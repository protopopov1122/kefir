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
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context, NULL));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_enum_type *enum_type = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(
        mem, context->type_bundle, "", context->type_traits->underlying_enumeration_type, &enum_type);
    REQUIRE_OK(kefir_ast_enumeration_type_constant_auto(mem, context->symbols, enum_type, "X"));
    REQUIRE_OK(kefir_ast_enumeration_type_constant_auto(mem, context->symbols, enum_type, "Y"));
    REQUIRE_OK(kefir_ast_enumeration_type_constant_auto(mem, context->symbols, enum_type, "Z"));

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void());

    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "bool", kefir_ast_type_boolean(), NULL,
                                                        NULL, NULL));
    REQUIRE_OK(
        kefir_ast_local_context_declare_external(mem, &local_context, "char", kefir_ast_type_char(), NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "uchar", kefir_ast_type_unsigned_char(),
                                                        NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "schar", kefir_ast_type_signed_char(),
                                                        NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "ushort", kefir_ast_type_unsigned_short(),
                                                        NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "sshort", kefir_ast_type_signed_short(),
                                                        NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "uint", kefir_ast_type_unsigned_int(),
                                                        NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "sint", kefir_ast_type_signed_int(), NULL,
                                                        NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "float", kefir_ast_type_float(), NULL,
                                                        NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "ulong", kefir_ast_type_unsigned_long(),
                                                        NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "slong", kefir_ast_type_signed_long(),
                                                        NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "ullong",
                                                        kefir_ast_type_unsigned_long_long(), NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "sllong",
                                                        kefir_ast_type_signed_long_long(), NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "double", kefir_ast_type_double(), NULL,
                                                        NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "enum", type1, NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "ptr", type2, NULL, NULL, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(
        kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context, context, &env, &module, NULL));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
        mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

#define SIMPLE_ASSIGN_NODE(_node1, _node2)                                                    \
    do {                                                                                      \
        struct kefir_ast_node_base *node =                                                    \
            KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(mem, (_node1), (_node2)));    \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));                               \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context)); \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));                                           \
    } while (0)

    FUNC("assign_bool", {
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "bool")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true)));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "bool")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1.0f)));
    });

    FUNC("assign_char", {
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "char")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A')));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "uchar")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'B')));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "schar")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'C')));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "schar")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.0)));
    });

    FUNC("assign_short", {
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ushort")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0xfe)));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sshort")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -0x4a)));
    });

    FUNC("assign_int", {
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "uint")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0x1ffff)));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sint")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -0x5544ef)));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "uint")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1e4)));
    });

    FUNC("assign_long", {
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ulong")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, -(~0l))));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "slong")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, ~0l)));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "slong")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1.0006f)));
    });

    FUNC("assign_long_long", {
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ullong")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 1990)));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sllong")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -65196362)));
    });

    FUNC("assign_float_double", {
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "float")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.514)));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "double")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, -1e4)));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "float")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A')));
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "double")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, -15)));
    });

    FUNC("enum", {
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "enum")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)));
    });

    FUNC("pointer", {
        SIMPLE_ASSIGN_NODE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
