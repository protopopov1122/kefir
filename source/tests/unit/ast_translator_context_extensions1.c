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
#include "kefir/ast-translator/context.h"
#include "kefir/test/util.h"

static kefir_result_t on_init(struct kefir_mem *mem, struct kefir_ast_translator_context *context) {
    context->extensions_payload = KEFIR_MALLOC(mem, 1024);
    *((void **) context->extensions_payload) = context;
    return KEFIR_OK;
}

static kefir_result_t on_free(struct kefir_mem *mem, struct kefir_ast_translator_context *context) {
    KEFIR_FREE(mem, context->extensions_payload);
    context->extensions_payload = NULL;
    return KEFIR_OK;
}

DEFINE_CASE(ast_translator_context_extensions1, "AST translator context - extensions #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_translator_environment env;
    struct kefir_ast_global_context context;
    struct kefir_ast_translator_context translator_context;
    struct kefir_ir_module module;

    struct kefir_ast_translator_context_extensions ext = {.on_init = on_init, .on_free = on_free};

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &env.target_env, &context, NULL));
    ASSERT_OK(kefir_ir_module_alloc(&kft_mem, &module));
    ASSERT_OK(kefir_ast_translator_context_init(&kft_mem, &translator_context, &context.context, &env, &module, &ext));

    ASSERT(translator_context.extensions_payload != NULL);
    ASSERT(*((void **) translator_context.extensions_payload) == &translator_context);

    ASSERT_OK(kefir_ast_translator_context_free(&kft_mem, &translator_context));
    ASSERT_OK(kefir_ir_module_free(&kft_mem, &module));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &context));
}
END_CASE

DEFINE_CASE(ast_translator_context_local_extensions1, "AST translator context - local extensions #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_translator_environment env;
    struct kefir_ast_global_context context;
    struct kefir_ast_translator_context translator_context, local_translator_context;
    struct kefir_ir_module module;

    struct kefir_ast_translator_context_extensions ext = {.on_init = on_init, .on_free = on_free};

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &env.target_env, &context, NULL));
    ASSERT_OK(kefir_ir_module_alloc(&kft_mem, &module));
    ASSERT_OK(kefir_ast_translator_context_init(&kft_mem, &translator_context, &context.context, &env, &module, &ext));
    ASSERT_OK(kefir_ast_translator_context_init_local(&kft_mem, &local_translator_context, &context.context,
                                                      &translator_context));

    ASSERT(translator_context.extensions_payload != NULL);
    ASSERT(*((void **) translator_context.extensions_payload) == &translator_context);
    ASSERT(local_translator_context.extensions_payload != NULL);
    ASSERT(*((void **) local_translator_context.extensions_payload) == &local_translator_context);

    ASSERT_OK(kefir_ast_translator_context_free(&kft_mem, &local_translator_context));
    ASSERT_OK(kefir_ast_translator_context_free(&kft_mem, &translator_context));
    ASSERT_OK(kefir_ir_module_free(&kft_mem, &module));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &context));
}
END_CASE
