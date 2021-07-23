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

#ifndef KEFIR_CODEGEN_CODEGEN_H_
#define KEFIR_CODEGEN_CODEGEN_H_

#include <stdio.h>
#include "kefir/core/mem.h"
#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"

typedef struct kefir_codegen {
    kefir_result_t (*translate)(struct kefir_mem *, struct kefir_codegen *, const struct kefir_ir_module *);
    kefir_result_t (*close)(struct kefir_codegen *);

    void *data;
} kefir_codegen_t;

#define KEFIR_CODEGEN_TRANSLATE(mem, codegen, module) ((codegen)->translate((mem), (codegen), (module)))
#define KEFIR_CODEGEN_CLOSE(codegen) ((codegen)->close((codegen)))

#endif
