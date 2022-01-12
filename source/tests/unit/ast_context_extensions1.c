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

#include "kefir/test/unit_test.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/function_declaration_context.h"
#include "kefir/preprocessor/ast_context.h"
#include "kefir/test/util.h"

static kefir_result_t on_init(struct kefir_mem *mem, struct kefir_ast_context *context) {
    context->extensions_payload = KEFIR_MALLOC(mem, 1024);
    *((void **) context->extensions_payload) = context;
    return KEFIR_OK;
}

static kefir_result_t on_free(struct kefir_mem *mem, struct kefir_ast_context *context) {
    KEFIR_FREE(mem, context->extensions_payload);
    context->extensions_payload = NULL;
    return KEFIR_OK;
}

DEFINE_CASE(ast_context_preprocessor_extensions1, "AST context - preprocessor context #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_translator_environment env;
    struct kefir_preprocessor_ast_context ast_context;

    struct kefir_ast_context_extensions ext = {.on_init = on_init, .on_free = on_free};

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    ASSERT_OK(kefir_preprocessor_ast_context_init(&kft_mem, &ast_context, &symbols, kefir_util_default_type_traits(),
                                                  &env.target_env, &ext));

    ASSERT(ast_context.context.extensions_payload != NULL);
    ASSERT(*((void **) ast_context.context.extensions_payload) == &ast_context.context);

    ASSERT_OK(kefir_preprocessor_ast_context_free(&kft_mem, &ast_context));
}
END_CASE

DEFINE_CASE(ast_context_global_extensions1, "AST context - global context #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_translator_environment env;
    struct kefir_ast_global_context context;

    struct kefir_ast_context_extensions ext = {.on_init = on_init, .on_free = on_free};

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &env.target_env, &context, &ext));

    ASSERT(context.context.extensions_payload != NULL);
    ASSERT(*((void **) context.context.extensions_payload) == &context.context);

    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &context));
}
END_CASE

DEFINE_CASE(ast_context_local_extensions1, "AST context - local context #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_translator_environment env;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    struct kefir_ast_context_extensions ext = {.on_init = on_init, .on_free = on_free};

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &env.target_env, &global_context, &ext));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT(context.context.extensions_payload != NULL);
    ASSERT(*((void **) context.context.extensions_payload) == &context.context);
    ASSERT(global_context.context.extensions_payload != NULL);
    ASSERT(*((void **) global_context.context.extensions_payload) == &global_context.context);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_context_function_decl_context_extensions1, "AST context - function declaration context #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_translator_environment env;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    struct kefir_ast_function_declaration_context context;

    struct kefir_ast_context_extensions ext = {.on_init = on_init, .on_free = on_free};

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &env.target_env, &global_context, &ext));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    ASSERT_OK(kefir_ast_function_declaration_context_init(&kft_mem, &local_context.context, &context));

    ASSERT(context.context.extensions_payload != NULL);
    ASSERT(*((void **) context.context.extensions_payload) == &context.context);
    ASSERT(local_context.context.extensions_payload != NULL);
    ASSERT(*((void **) local_context.context.extensions_payload) == &local_context.context);
    ASSERT(global_context.context.extensions_payload != NULL);
    ASSERT(*((void **) global_context.context.extensions_payload) == &global_context.context);

    ASSERT_OK(kefir_ast_function_declaration_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
