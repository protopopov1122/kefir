/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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

#ifndef KEFIR_CODEGEN_AMD64_LABELS_H_
#define KEFIR_CODEGEN_AMD64_LABELS_H_

#include "kefir/core/basic-types.h"

#define KEFIR_AMD64_SYSV_PROCEDURE_LABEL "%s"
#define KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL "__%s_body"
#define KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL "__%s_epilogue"
#define KEFIR_AMD64_SYSV_FUNCTION_GATE_NAMED_LABEL "__kefirrt_sfunction_%s_gate"
#define KEFIR_AMD64_SYSV_FUNCTION_GATE_ID_LABEL "__kefirrt_ifunction_gate" KEFIR_ID_FMT
#define KEFIR_AMD64_SYSV_FUNCTION_VIRTUAL_GATE_NAMED_LABEL "__kefirrt_sfunction_%s_vgate"
#define KEFIR_AMD64_SYSV_FUNCTION_VIRTUAL_GATE_ID_LABEL "__kefirrt_ifunction_vgate" KEFIR_ID_FMT
#define KEFIR_AMD64_SYSV_FUNCTION_VARARG_START_LABEL "__%s_vararg_start"
#define KEFIR_AMD64_SYSV_FUNCTION_VARARG_END_LABEL "__%s_vararg_end"
#define KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL "__%s_vararg_type" KEFIR_UINT32_FMT "_" KEFIR_UINT32_FMT
#define KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_SCALAR_LABEL "__%s_vararg_" KEFIR_INT_FMT "_type"

#endif
