/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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

#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_BITFIELDS_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_BITFIELDS_H_

#include "kefir/ir/bitfields.h"

kefir_result_t kefir_codegen_amd64_sysv_bitfield_allocator(struct kefir_mem *, struct kefir_ir_type *,
                                                           struct kefir_ir_bitfield_allocator *);

#endif
