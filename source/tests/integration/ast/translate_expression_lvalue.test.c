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

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context, NULL));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

#define LVALUE(_node)                                                                                      \
    do {                                                                                                   \
        struct kefir_ast_node_base *node =                                                                 \
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(mem, KEFIR_AST_OPERATION_ADDRESS, (_node))); \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));                                            \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));              \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));                                                        \
    } while (0)

    struct kefir_ast_struct_type *struct_type = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(mem, context->type_bundle, "", &struct_type);
    REQUIRE_OK(
        kefir_ast_struct_type_field(mem, context->symbols, struct_type, "fiel1", kefir_ast_type_unsigned_char(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type, "fiel2",
                                           kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_double(),
                                                                kefir_ast_constant_expression_integer(mem, 4), NULL),
                                           NULL));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(mem, context->type_bundle, "", &struct_type2);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type2, "ptr",
                                           kefir_ast_type_pointer(mem, context->type_bundle, type1), NULL));

    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context, "variable1", kefir_ast_type_signed_int(),
                                                         NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context, "variable2", kefir_ast_type_signed_char(),
                                                        NULL, NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_global_context_define_static(
        mem, &global_context, "variable3",
        kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_boolean(),
                             kefir_ast_constant_expression_integer(mem, 3), NULL),
        NULL, NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(
        mem, &local_context, "variable4", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_float()),
        NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_static(mem, &local_context, "variable5", type1, NULL, NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context, "variable6",
                                                   kefir_ast_type_pointer(mem, context->type_bundle, type2), NULL, NULL,
                                                   NULL, NULL));

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

    FUNC("identifier", {
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable1")));
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable2")));
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable3")));
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable4")));
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable5")));
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable6")));
    });

    FUNC("array_subscript", {
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable3")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)))));
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable4")))));
    });

    FUNC("struct_field", {
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(
            mem, context->symbols, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable5")),
            "fiel1")));
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(
            mem, context->symbols, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable5")),
            "fiel2")));
    });

    FUNC("struct_field_indirect", {
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
            mem, context->symbols,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
                mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "variable6")), "ptr")),
            "fiel2")));
    });

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers,
                                                          kefir_ast_type_specifier_int(mem)));

    FUNC("indirection", {
        LVALUE(KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            mem, KEFIR_AST_OPERATION_INDIRECTION,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))))));
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
