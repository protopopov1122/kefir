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

#include "kefir/ast-translator/context.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/extensions.h"

kefir_result_t kefir_ast_translator_context_init(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                 const struct kefir_ast_context *ast_context,
                                                 const struct kefir_ast_translator_environment *environment,
                                                 struct kefir_ir_module *module,
                                                 const struct kefir_ast_translator_context_extensions *extensions) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected a pointer to valid AST translator context"));
    REQUIRE(ast_context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(environment != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator environment"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));

    context->base_context = NULL;
    context->ast_context = ast_context;
    context->environment = environment;
    context->module = module;
    context->global_scope_layout = NULL;
    context->local_scope_layout = NULL;

    context->extensions = extensions;
    context->extensions_payload = NULL;
    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, context, on_init);
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_context_init_local(struct kefir_mem *mem,
                                                       struct kefir_ast_translator_context *context,
                                                       const struct kefir_ast_context *ast_context,
                                                       struct kefir_ast_translator_context *base_context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected a pointer to valid AST translator context"));
    REQUIRE(ast_context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(base_context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid base AST translator context"));

    context->base_context = base_context;
    context->ast_context = ast_context;
    context->environment = base_context->environment;
    context->module = base_context->module;
    context->global_scope_layout = NULL;
    context->local_scope_layout = NULL;

    context->extensions = base_context->extensions;
    context->extensions_payload = NULL;
    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, context, on_init);
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_context_free(struct kefir_mem *mem, struct kefir_ast_translator_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected a pointer to valid AST translator context"));

    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, context, on_free);
    REQUIRE_OK(res);

    context->base_context = NULL;
    context->ast_context = NULL;
    context->environment = NULL;
    context->module = NULL;
    context->global_scope_layout = NULL;
    context->local_scope_layout = NULL;
    return KEFIR_OK;
}
