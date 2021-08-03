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

#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_VARARG_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_VARARG_H_

#include "kefir/ir/builtins.h"
#include "kefir/codegen/amd64/system-v/abi/data.h"
#include "kefir/codegen/amd64/system-v/abi/builtins.h"
#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_codegen_amd64_sysv_vararg_instruction(struct kefir_mem *, struct kefir_codegen_amd64 *,
                                                           struct kefir_codegen_amd64_sysv_module *,
                                                           struct kefir_amd64_sysv_function *,
                                                           const struct kefir_irinstr *);

#endif
