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

#include "kefir/codegen/amd64/system-v/abi/tls.h"
#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"

kefir_result_t kefir_amd64_sysv_thread_local_reference(struct kefir_codegen_amd64 *codegen, const char *identifier,
                                                       kefir_bool_t local) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AMD64 codegen"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid TLS identifier"));

    if (!codegen->config->emulated_tls) {
        if (local) {
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_THREAD_LOCAL, identifier);
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_FS "0");
        } else {
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_QWORD KEFIR_AMD64_FS "0");
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_QWORD KEFIR_AMD64_THREAD_LOCAL_GOT, identifier);
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
        }
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    } else {
        if (local) {
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_THREAD_LOCAL_EMUTLS, identifier);
        } else {
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_QWORD KEFIR_AMD64_THREAD_LOCAL_EMUTLS_GOT, identifier);
        }
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CALL);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_EMUTLS_GET_ADDR);
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    }

    return KEFIR_OK;
}
