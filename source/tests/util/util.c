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

#include "kefir/test/util.h"
#include "kefir/codegen/amd64/system-v/platform.h"

static kefir_bool_t init_done = false;
static struct kefir_ir_target_platform IR_TARGET;
static struct kefir_ast_translator_environment TRANSLATOR_ENV;

struct kefir_ir_target_platform *kft_util_get_ir_target_platform() {
    if (!init_done) {
        REQUIRE(kefir_codegen_amd64_sysv_target_platform(&IR_TARGET) == KEFIR_OK, NULL);
        REQUIRE(kefir_ast_translator_environment_init(&TRANSLATOR_ENV, &IR_TARGET) == KEFIR_OK, NULL);
        init_done = true;
    }
    return &IR_TARGET;
}

struct kefir_ast_translator_environment *kft_util_get_translator_environment() {
    if (!init_done) {
        REQUIRE(kefir_codegen_amd64_sysv_target_platform(&IR_TARGET) == KEFIR_OK, NULL);
        REQUIRE(kefir_ast_translator_environment_init(&TRANSLATOR_ENV, &IR_TARGET) == KEFIR_OK, NULL);
        init_done = true;
    }
    return &TRANSLATOR_ENV;
}

const struct kefir_ast_type_traits *kefir_util_default_type_traits() {
    static struct kefir_ast_type_traits DEFAULT_TYPE_TRAITS;
    static kefir_bool_t DEFAULT_TYPE_TRAITS_INIT_DONE = false;
    if (!DEFAULT_TYPE_TRAITS_INIT_DONE) {
        kefir_ast_type_traits_init(KEFIR_DATA_MODEL_LP64, &DEFAULT_TYPE_TRAITS);
        DEFAULT_TYPE_TRAITS_INIT_DONE = true;
    }
    return &DEFAULT_TYPE_TRAITS;
}
