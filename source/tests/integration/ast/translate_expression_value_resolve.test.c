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
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_enum_type *enum_type = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(
        mem, context->type_bundle, "", context->type_traits->underlying_enumeration_type, &enum_type);
    const struct kefir_ast_type *type2 =
        kefir_ast_type_qualified(mem, context->type_bundle, kefir_ast_type_signed_short(),
                                 (struct kefir_ast_type_qualification){.volatile_type = true});

    struct kefir_ast_struct_type *struct_type = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(mem, context->type_bundle, "", &struct_type);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type, "x", kefir_ast_type_bool(), NULL));

    struct kefir_ast_struct_type *union_type = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_union(mem, context->type_bundle, "", &union_type);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, union_type, "y", kefir_ast_type_float(), NULL));

    const struct kefir_ast_type *type5 = kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_signed_int(),
                                                              kefir_ast_constant_expression_integer(mem, 2), NULL);
    const struct kefir_ast_type *type6 = kefir_ast_type_unbounded_array(mem, context->type_bundle, type4, NULL);

    struct kefir_ast_function_type *function_type = NULL;
    const struct kefir_ast_type *type7 =
        kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_void(), &function_type);
    REQUIRE_OK(kefir_ast_global_context_declare_function(mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                         "func1", type7, NULL, NULL));

    struct {
        const char *identifier;
        const struct kefir_ast_type *type;
    } VARS[] = {
        {"boolean", kefir_ast_type_bool()},
        {"char", kefir_ast_type_char()},
        {"unsigned_char", kefir_ast_type_unsigned_char()},
        {"signed_char", kefir_ast_type_signed_char()},
        {"unsigned_short", kefir_ast_type_unsigned_short()},
        {"signed_short", kefir_ast_type_signed_short()},
        {"unsigned_int", kefir_ast_type_unsigned_int()},
        {"signed_int", kefir_ast_type_signed_int()},
        {"unsigned_long", kefir_ast_type_unsigned_long()},
        {"signed_long", kefir_ast_type_signed_long()},
        {"unsigned_long_long", kefir_ast_type_unsigned_long_long()},
        {"signed_long_long", kefir_ast_type_signed_long_long()},
        {"float", kefir_ast_type_float()},
        {"double", kefir_ast_type_double()},
        {"void_ptr", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void())},
        {"enum", type1},
        {"qualified", type2},
        {"structure", type3},
        {"union", type4},
        {"array1", type5},
        {"array2", type6},
    };
    const kefir_size_t VAR_LENGTH = sizeof(VARS) / sizeof(VARS[0]);

    for (kefir_size_t i = 0; i < VAR_LENGTH; i++) {
        REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context, VARS[i].identifier, VARS[i].type,
                                                            NULL, NULL, NULL, NULL));
    }

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

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

    struct kefir_ir_type *func1_params = kefir_ir_module_new_type(mem, &module, 0, NULL);
    struct kefir_ir_type *func1_returns = kefir_ir_module_new_type(mem, &module, 0, NULL);

    struct kefir_ir_function_decl *func1_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func1", func1_params, false, func1_returns);
    REQUIRE(func1_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func1 =
        kefir_ir_module_new_function(mem, &module, func1_decl, translator_local_scope.local_layout, 0);
    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func1->body));

    for (kefir_size_t i = 0; i < VAR_LENGTH; i++) {
        struct kefir_ast_node_base *node1 =
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, VARS[i].identifier));
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node1));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node1, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node1));
    }

    struct kefir_ast_node_base *node2 = KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "func1"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node2));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node2, &builder, &translator_context));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node2));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
