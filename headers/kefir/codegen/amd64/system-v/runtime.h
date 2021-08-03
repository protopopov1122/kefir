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

#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_RUNTIME_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_RUNTIME_H_

#include "kefir/core/basic-types.h"

#define KEFIR_AMD64_SYSTEM_V_RUNTIME_PRESERVE_STATE "__kefirrt_preserve_state"
#define KEFIR_AMD64_SYSTEM_V_RUNTIME_RESTORE_STATE "__kefirrt_restore_state"
#define KEFIR_AMD64_SYSTEM_V_RUNTIME_SAVE_REGISTERS "__kefirrt_save_registers"
#define KEFIR_AMD64_SYSTEM_V_RUNTIME_VARARG_INT "__kefirrt_load_integer_vararg"
#define KEFIR_AMD64_SYSTEM_V_RUNTIME_VARARG_SSE "__kefirrt_load_sse_vararg"
#define KEFIR_AMD64_SYSTEM_V_RUNTIME_VARARG_COPY "__kefirrt_copy_vararg"
#define KEFIR_AMD64_SYSTEM_V_RUNTIME_STRING_LITERAL "__kefirrt_string_literal" KEFIR_ID_FMT

extern const char *KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOLS[];
extern kefir_size_t KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOL_COUNT;

#endif
