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

#include <stdlib.h>
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ir_function_decl_alloc(struct kefir_mem *mem, kefir_id_t id, const char *name,
                                            struct kefir_ir_type *parameters, kefir_id_t parameters_type_id,
                                            bool vararg, struct kefir_ir_type *returns, kefir_id_t returns_type_id,
                                            struct kefir_ir_function_decl *decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocation"));
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR function declaration pointer"));
    REQUIRE(name == NULL || strlen(name) > 0,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "IR function declaration identifier cannot be an empty string"));

    decl->id = id;
    decl->name = name;
    decl->params = parameters;
    decl->params_type_id = parameters_type_id;
    decl->result = returns;
    decl->result_type_id = returns_type_id;
    decl->vararg = vararg;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_function_decl_free(struct kefir_mem *mem, struct kefir_ir_function_decl *decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocation"));
    ;
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR function declaration pointer"));
    decl->result = NULL;
    decl->params = NULL;
    decl->name = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_function_alloc(struct kefir_mem *mem, struct kefir_ir_function_decl *decl,
                                       struct kefir_ir_type *locals, kefir_size_t bodySz,
                                       struct kefir_ir_function *func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocation"));
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR function declaration"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR function pointer"));
    REQUIRE(decl->name != NULL && strlen(decl->name) > 0,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid declaration name"));

    func->name = decl->name;
    func->declaration = decl;
    func->locals = locals;
    kefir_result_t result = kefir_irblock_alloc(mem, bodySz, &func->body);
    REQUIRE_ELSE(result == KEFIR_OK, {
        kefir_ir_function_decl_free(mem, func->declaration);
        return result;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ir_function_free(struct kefir_mem *mem, struct kefir_ir_function *func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR function pointer"));
    kefir_irblock_free(mem, &func->body);
    func->locals = NULL;
    func->declaration = NULL;
    return KEFIR_OK;
}
