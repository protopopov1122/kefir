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

#ifndef KEFIR_IR_BITFIELDS_H_
#define KEFIR_IR_BITFIELDS_H_

#include "kefir/core/basic-types.h"
#include "kefir/ir/type.h"
#include "kefir/ir/builder.h"

typedef struct kefir_ir_bitfield {
    kefir_uint8_t offset;
    kefir_uint8_t width;
} kefir_ir_bitfield_t;

typedef struct kefir_ir_bitfield_allocator {
    kefir_bool_t (*has_bitfield_run)(struct kefir_ir_bitfield_allocator *);
    kefir_result_t (*reset)(struct kefir_ir_bitfield_allocator *);
    kefir_result_t (*next)(struct kefir_mem *, struct kefir_ir_bitfield_allocator *, kefir_size_t, kefir_bool_t,
                           kefir_ir_typecode_t, uint8_t, struct kefir_ir_typeentry *, struct kefir_ir_bitfield *);
    kefir_result_t (*next_colocated)(struct kefir_mem *, struct kefir_ir_bitfield_allocator *, kefir_bool_t,
                                     kefir_ir_typecode_t, uint8_t, struct kefir_ir_typeentry *,
                                     struct kefir_ir_bitfield *);
    kefir_result_t (*free)(struct kefir_mem *, struct kefir_ir_bitfield_allocator *);

    void *payload;
} kefir_ir_bitfield_allocator_t;

#define KEFIR_IR_BITFIELD_ALLOCATOR_HAS_BITFIELD_RUN(allocator) ((allocator)->has_bitfield_run((allocator)))
#define KEFIR_IR_BITFIELD_ALLOCATOR_RESET(allocator) ((allocator)->reset((allocator)))
#define KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(mem, allocator, struct_index, named, typecode, width, typeentry, bitfield) \
    ((allocator)->next((mem), (allocator), (struct_index), (named), (typecode), (width), (typeentry), (bitfield)))
#define KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(mem, allocator, named, typecode, width, typeentry, bitfield) \
    ((allocator)->next_colocated((mem), (allocator), (named), (typecode), (width), (typeentry), (bitfield)))
#define KEFIR_IR_BITFIELD_ALLOCATOR_FREE(mem, allocator) ((allocator)->free((mem), (allocator)))

#endif
