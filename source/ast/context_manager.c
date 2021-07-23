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

#include "kefir/ast/context_manager.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_context_manager_init(struct kefir_ast_global_context *global_context,
                                              struct kefir_ast_context_manager *context_mgr) {
    REQUIRE(global_context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global context"));
    REQUIRE(context_mgr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context manager"));

    context_mgr->global = global_context;
    context_mgr->local = NULL;
    context_mgr->current = &global_context->context;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_manager_attach_local(struct kefir_ast_local_context *local_context,
                                                      struct kefir_ast_context_manager *context_mgr) {
    REQUIRE(local_context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local context"));
    REQUIRE(context_mgr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context manager"));
    REQUIRE(context_mgr->local == NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Context manager already has attached local context"));

    context_mgr->local = local_context;
    context_mgr->current = &local_context->context;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_manager_detach_local(struct kefir_ast_context_manager *context_mgr) {
    REQUIRE(context_mgr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context manager"));
    REQUIRE(context_mgr->local != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Context manager has no attached local context"));

    context_mgr->local = NULL;
    context_mgr->current = &context_mgr->global->context;
    return KEFIR_OK;
}
