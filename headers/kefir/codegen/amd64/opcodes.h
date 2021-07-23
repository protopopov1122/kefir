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

#ifndef KEFIR_CODEGEN_AMD64_OPCODES_H_
#define KEFIR_CODEGEN_AMD64_OPCODES_H_

#include "kefir/core/basic-types.h"
#include "kefir/ir/opcodes.h"

const char *kefir_amd64_iropcode_handler(kefir_iropcode_t);
kefir_result_t kefir_amd64_iropcode_handler_list(kefir_result_t (*)(kefir_iropcode_t, const char *, void *), void *);

#endif
