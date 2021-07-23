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

#ifndef KEFIR_IR_BUILDER_H_
#define KEFIR_IR_BUILDER_H_

#include "kefir/core/mem.h"
#include "kefir/ir/type.h"
#include "kefir/ir/instr.h"

kefir_result_t kefir_irbuilder_type_append(struct kefir_mem *, struct kefir_ir_type *,
                                           const struct kefir_ir_typeentry *);
kefir_result_t kefir_irbuilder_type_append_v(struct kefir_mem *, struct kefir_ir_type *, kefir_ir_typecode_t,
                                             kefir_uint32_t, kefir_int64_t);
kefir_result_t kefir_irbuilder_type_append_e(struct kefir_mem *, struct kefir_ir_type *, const struct kefir_ir_type *,
                                             kefir_size_t);

kefir_result_t kefir_irbuilder_block_appendi64(struct kefir_mem *, struct kefir_irblock *, kefir_iropcode_t,
                                               kefir_int64_t);
kefir_result_t kefir_irbuilder_block_appendu64(struct kefir_mem *, struct kefir_irblock *, kefir_iropcode_t,
                                               kefir_uint64_t);
kefir_result_t kefir_irbuilder_block_appendi32(struct kefir_mem *, struct kefir_irblock *, kefir_iropcode_t,
                                               kefir_int32_t, kefir_int32_t);
kefir_result_t kefir_irbuilder_block_appendu32(struct kefir_mem *, struct kefir_irblock *, kefir_iropcode_t,
                                               kefir_uint32_t, kefir_uint32_t);
kefir_result_t kefir_irbuilder_block_appendf64(struct kefir_mem *, struct kefir_irblock *, kefir_iropcode_t, double);
kefir_result_t kefir_irbuilder_block_appendf32(struct kefir_mem *, struct kefir_irblock *, kefir_iropcode_t, float,
                                               float);

typedef struct kefir_irbuilder_block {
    struct kefir_irblock *block;
    void *payload;

    kefir_result_t (*appendi64)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_int64_t);
    kefir_result_t (*appendu64)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_uint64_t);
    kefir_result_t (*appendi32)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_int32_t, kefir_int32_t);
    kefir_result_t (*appendu32)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_uint32_t, kefir_uint32_t);
    kefir_result_t (*appendf64)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_float64_t);
    kefir_result_t (*appendf32)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_float32_t, kefir_float32_t);
    kefir_result_t (*free)(struct kefir_irbuilder_block *);
} kefir_irbuilder_block_t;

kefir_result_t kefir_irbuilder_block_init(struct kefir_mem *, struct kefir_irbuilder_block *, struct kefir_irblock *);

#define KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, opcode, arg) ((builder)->appendi64((builder), (opcode), (arg)))
#define KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, opcode, arg) ((builder)->appendu64((builder), (opcode), (arg)))
#define KEFIR_IRBUILDER_BLOCK_APPENDI32(builder, opcode, arg1, arg2) \
    ((builder)->appendi32((builder), (opcode), (arg1), (arg2)))
#define KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, opcode, arg1, arg2) \
    ((builder)->appendu32((builder), (opcode), (arg1), (arg2)))
#define KEFIR_IRBUILDER_BLOCK_APPENDF64(builder, opcode, arg) ((builder)->appendf64((builder), (opcode), (arg)))
#define KEFIR_IRBUILDER_BLOCK_APPENDF32(builder, opcode, arg1, arg2) \
    ((builder)->appendf32((builder), (opcode), (arg1), (arg2)))
#define KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) (kefir_irblock_length((builder)->block))
#define KEFIR_IRBUILDER_BLOCK_INSTR_AT(builder, index) (kefir_irblock_at((builder)->block, (index)))
#define KEFIR_IRBUILDER_BLOCK_FREE(builder) ((builder)->free((builder)))

typedef struct kefir_irbuilder_type {
    struct kefir_ir_type *type;
    void *payload;

    kefir_result_t (*append)(struct kefir_irbuilder_type *, const struct kefir_ir_typeentry *);
    kefir_result_t (*append_v)(struct kefir_irbuilder_type *, kefir_ir_typecode_t, kefir_uint32_t, kefir_int64_t);
    kefir_result_t (*append_e)(struct kefir_irbuilder_type *, const struct kefir_ir_type *, kefir_size_t);
    kefir_result_t (*free)(struct kefir_irbuilder_type *);
} kefir_irbuilder_type_t;

kefir_result_t kefir_irbuilder_type_init(struct kefir_mem *, struct kefir_irbuilder_type *, struct kefir_ir_type *);

#define KEFIR_IRBUILDER_TYPE_APPEND(builder, typeentry) ((builder)->append((builder), (typeentry)))
#define KEFIR_IRBUILDER_TYPE_APPEND_V(builder, typecode, alignment, param) \
    ((builder)->append_v((builder), (typecode), (alignment), (param)))
#define KEFIR_IRBUILDER_TYPE_APPEND_E(builder, type, index) ((builder)->append_v((builder), (type), (index)))
#define KEFIR_IRBUILDER_TYPE_FREE(builder) ((builder)->free((builder)))

#endif
