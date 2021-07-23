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

#ifndef KEFIR_IR_FORMAT_H_
#define KEFIR_IR_FORMAT_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"
#include "kefir/util/json.h"

kefir_result_t kefir_ir_format_module(FILE *, const struct kefir_ir_module *);
kefir_result_t kefir_ir_format_module_json(struct kefir_json_output *, const struct kefir_ir_module *);
kefir_result_t kefir_ir_format_type(FILE *, const struct kefir_ir_type *);
kefir_result_t kefir_ir_format_type_json(struct kefir_json_output *, const struct kefir_ir_type *);

#endif
