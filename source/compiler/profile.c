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

#include "kefir/compiler/profile.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/codegen/amd64/system-v/platform.h"
#include "kefir/codegen/amd64-sysv.h"

static kefir_result_t amd64_sysv_new_codegen(struct kefir_mem *mem, FILE *output, struct kefir_codegen **codegen_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(output != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid FILE"));
    REQUIRE(codegen_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to code generator"));

    struct kefir_codegen_amd64 *codegen = KEFIR_MALLOC(mem, sizeof(struct kefir_codegen_amd64));
    REQUIRE(codegen != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AMD64 System-V code generator"));
    kefir_result_t res = kefir_codegen_amd64_sysv_init(codegen, output);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, codegen);
        return res;
    });
    *codegen_ptr = &codegen->iface;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_free_codegen(struct kefir_mem *mem, struct kefir_codegen *codegen) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid code generator"));

    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(codegen));
    KEFIR_FREE(mem, codegen->self);
    return KEFIR_OK;
}

static kefir_result_t kefir_compiler_amd64_sysv_profile(struct kefir_compiler_profile *profile) {
    REQUIRE(profile != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler profile"));

    REQUIRE_OK(kefir_parser_integral_types_default(&profile->parser_integrals));
    REQUIRE_OK(kefir_codegen_amd64_sysv_target_platform(&profile->ir_target_platform));
    profile->type_traits = kefir_ast_default_type_traits();
    profile->new_codegen = amd64_sysv_new_codegen;
    profile->free_codegen = amd64_sysv_free_codegen;
    return KEFIR_OK;
}

const struct Profile {
    const char *identifier;
    kefir_result_t (*init)(struct kefir_compiler_profile *);
} Profiles[] = {{"amd64-sysv-nasm", kefir_compiler_amd64_sysv_profile}, {NULL, kefir_compiler_amd64_sysv_profile}};
const kefir_size_t ProfileCount = sizeof(Profiles) / sizeof(Profiles[0]);

kefir_result_t kefir_compiler_profile(struct kefir_compiler_profile *profile, const char *identifier) {
    REQUIRE(profile != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler profile"));

    for (kefir_size_t i = 0; i < ProfileCount; i++) {
        const struct Profile *profileId = &Profiles[i];
        if ((identifier != NULL && profileId->identifier != NULL && strcmp(identifier, profileId->identifier) == 0) ||
            (identifier == NULL && profileId->identifier == NULL)) {
            REQUIRE_OK(profileId->init(profile));
            return KEFIR_OK;
        }
    }
    return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Unable to find requested compiler profile");
}
