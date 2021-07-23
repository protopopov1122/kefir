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

#include "kefir/codegen/amd64/system-v/runtime.h"

const char *KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOLS[] = {
    KEFIR_AMD64_SYSTEM_V_RUNTIME_PRESERVE_STATE, KEFIR_AMD64_SYSTEM_V_RUNTIME_RESTORE_STATE,
    KEFIR_AMD64_SYSTEM_V_RUNTIME_SAVE_REGISTERS, KEFIR_AMD64_SYSTEM_V_RUNTIME_VARARG_INT,
    KEFIR_AMD64_SYSTEM_V_RUNTIME_VARARG_SSE,     KEFIR_AMD64_SYSTEM_V_RUNTIME_VARARG_COPY};

kefir_size_t KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOL_COUNT =
    sizeof(KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOLS) / sizeof(KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOLS[0]);
