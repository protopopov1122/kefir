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

#ifndef KEFIR_IR_FUNCTION_H_
#define KEFIR_IR_FUNCTION_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"
#include "kefir/ir/type.h"
#include "kefir/ir/instr.h"

typedef struct kefir_ir_function_decl {
    kefir_id_t id;
    const char *name;
    struct kefir_ir_type *params;
    bool vararg;
    struct kefir_ir_type *result;
} kefir_ir_function_decl_t;

typedef struct kefir_ir_function {
    const char *name;
    struct kefir_ir_function_decl *declaration;
    struct kefir_ir_type *locals;
    struct kefir_irblock body;
} kefir_ir_function_t;

kefir_result_t kefir_ir_function_decl_alloc(struct kefir_mem *, kefir_id_t, const char *, struct kefir_ir_type *, bool,
                                            struct kefir_ir_type *, struct kefir_ir_function_decl *);
kefir_result_t kefir_ir_function_decl_free(struct kefir_mem *, struct kefir_ir_function_decl *);

kefir_result_t kefir_ir_function_alloc(struct kefir_mem *, struct kefir_ir_function_decl *, struct kefir_ir_type *,
                                       kefir_size_t, struct kefir_ir_function *);
kefir_result_t kefir_ir_function_free(struct kefir_mem *, struct kefir_ir_function *);

#endif
