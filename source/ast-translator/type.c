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
#include "kefir/ast-translator/type.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/layout.h"
#include "kefir/ir/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_type_new(struct kefir_mem *mem, const struct kefir_ast_translator_environment *env,
                                             struct kefir_ir_module *module, const struct kefir_ast_type *type,
                                             kefir_size_t alignment,
                                             struct kefir_ast_translator_type **translator_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator environment"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));
    REQUIRE(translator_type != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer AST translator type"));

    kefir_result_t res = KEFIR_OK;
    struct kefir_ast_translator_type *tr_type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_type));
    REQUIRE(tr_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator cached type"));
    tr_type->type = type;

    tr_type->object.ir_type = kefir_ir_module_new_type(mem, module, 0, &tr_type->object.ir_type_id);
    struct kefir_irbuilder_type type_builder;
    res = kefir_irbuilder_type_init(mem, &type_builder, tr_type->object.ir_type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&type_builder);
        KEFIR_FREE(mem, tr_type);
        return res;
    });

    res = kefir_ast_translate_object_type(mem, type, alignment, env, &type_builder, &tr_type->object.layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&type_builder);
        KEFIR_FREE(mem, tr_type);
        return res;
    });

    res = KEFIR_IRBUILDER_TYPE_FREE(&type_builder);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_type_layout_free(mem, tr_type->object.layout);
        KEFIR_FREE(mem, tr_type);
        return res;
    });

    res = kefir_ast_translator_evaluate_type_layout(mem, env, tr_type->object.layout, tr_type->object.ir_type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_type_layout_free(mem, tr_type->object.layout);
        KEFIR_FREE(mem, tr_type);
        return res;
    });

    *translator_type = tr_type;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_free(struct kefir_mem *mem,
                                              struct kefir_ast_translator_type *translator_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(translator_type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid translator type"));

    if (translator_type->object.layout != NULL) {
        REQUIRE_OK(kefir_ast_type_layout_free(mem, translator_type->object.layout));
    }
    translator_type->object.layout = NULL;
    translator_type->object.ir_type = NULL;
    translator_type->object.ir_type_id = 0;
    KEFIR_FREE(mem, translator_type);
    return KEFIR_OK;
}
