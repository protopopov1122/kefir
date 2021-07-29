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

#include "kefir/codegen/amd64/system-v/bitfields.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct bitfield_allocator_payload {
    struct kefir_ir_type *ir_type;
    kefir_bool_t has_last_bitfield;
    struct {
        kefir_size_t head_offset;
        kefir_size_t tail_offset;
    } props;
};

static kefir_bool_t amd64_sysv_bitfield_has_bitfield_run(struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(allocator != NULL, false);
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload, allocator->payload);

    return payload->has_last_bitfield;
}

static kefir_result_t amd64_sysv_bitfield_reset(struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload, allocator->payload);
    payload->has_last_bitfield = false;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_props(kefir_ir_typecode_t typecode, kefir_size_t *size,
                                                kefir_size_t *alignment) {
    switch (typecode) {
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_BOOL:
        case KEFIR_IR_TYPE_INT8:
            ASSIGN_PTR(size, 8);
            ASSIGN_PTR(alignment, 8);
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            ASSIGN_PTR(size, 16);
            ASSIGN_PTR(alignment, 16);
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            ASSIGN_PTR(size, 32);
            ASSIGN_PTR(alignment, 32);
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_WORD:
        case KEFIR_IR_TYPE_INT64:
            ASSIGN_PTR(size, 64);
            ASSIGN_PTR(alignment, 64);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected bit-field type");
    }
    return KEFIR_OK;
}

struct struct_layout_visitor {
    struct kefir_vector layout;
    kefir_size_t *offset;
};

static kefir_result_t visit_struct_layout(const struct kefir_ir_type *type, kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct struct_layout_visitor *, visitor_payload, payload);

    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&visitor_payload->layout, index));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Failed to fetch IR type layout"));
    *visitor_payload->offset = MAX(*visitor_payload->offset, layout->relative_offset + layout->size);
    return KEFIR_OK;
}

static kefir_result_t struct_current_offset(struct kefir_mem *mem, const struct kefir_ir_type *type,
                                            kefir_size_t struct_index, kefir_size_t *offset) {
    const struct kefir_ir_typeentry *struct_typeentry = kefir_ir_type_at(type, struct_index);
    REQUIRE(struct_typeentry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR structure type index"));
    if (struct_typeentry->param > 0) {
        struct kefir_ir_type_visitor visitor;
        struct struct_layout_visitor payload = {.offset = offset};
        REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, &visit_struct_layout));
        REQUIRE_OK(kefir_amd64_sysv_type_layout_of(mem, type, struct_index, 1, &payload.layout));
        kefir_result_t res =
            kefir_ir_type_visitor_list_nodes(type, &visitor, &payload, struct_index + 1, struct_typeentry->param);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_vector_free(mem, &payload.layout);
            return res;
        });
        REQUIRE_OK(kefir_vector_free(mem, &payload.layout));
    } else {
        *offset = 0;
    }
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_next(struct kefir_mem *mem, struct kefir_ir_bitfield_allocator *allocator,
                                               kefir_size_t struct_index, kefir_bool_t named,
                                               kefir_ir_typecode_t base_typecode, uint8_t bitwidth,
                                               struct kefir_ir_typeentry *typeentry,
                                               struct kefir_ir_bitfield *bitfield) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    REQUIRE(bitwidth != 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero bit-field width"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type entry"));
    REQUIRE(bitfield != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR bitfield"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload, allocator->payload);

    const struct kefir_ir_typeentry *struct_typeentry = kefir_ir_type_at(payload->ir_type, struct_index);
    REQUIRE(struct_typeentry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR structure type index"));

    kefir_size_t current_bit_offset = 0;
    REQUIRE_OK(struct_current_offset(mem, payload->ir_type, struct_index, &current_bit_offset));
    current_bit_offset *= 8;

    kefir_size_t base_bit_size = 0;
    kefir_size_t base_bit_alignment = 0;
    REQUIRE_OK(amd64_sysv_bitfield_props(base_typecode, &base_bit_size, &base_bit_alignment));

    REQUIRE(bitwidth <= base_bit_size,
            KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Bit-field width exceeds storage unit width"));

    payload->has_last_bitfield = true;
    payload->props.head_offset = current_bit_offset;
    payload->props.tail_offset = current_bit_offset;
    if ((current_bit_offset & (base_bit_alignment - 1)) + bitwidth > base_bit_size) {
        payload->props.tail_offset = kefir_codegen_pad_aligned(current_bit_offset, base_bit_alignment);
    }

    bitfield->offset = payload->props.tail_offset - payload->props.head_offset;
    bitfield->width = bitwidth;
    payload->props.tail_offset += bitwidth;

    typeentry->typecode = KEFIR_IR_TYPE_BITS;
    if (named) {
        typeentry->alignment = base_bit_alignment / 8;
    } else {
        typeentry->alignment = 0;
    }
    typeentry->param = payload->props.tail_offset - payload->props.head_offset;

    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_next_colocated(struct kefir_mem *mem,
                                                         struct kefir_ir_bitfield_allocator *allocator,
                                                         kefir_bool_t named, kefir_ir_typecode_t colocated_base,
                                                         uint8_t bitwidth, struct kefir_ir_typeentry *typeentry,
                                                         struct kefir_ir_bitfield *bitfield) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    REQUIRE(bitwidth != 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero bit-field width"));
    REQUIRE(bitfield != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR bitfield"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload, allocator->payload);
    REQUIRE(payload->has_last_bitfield, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to colocate a bit-field"));

    kefir_size_t colocated_bit_size = 0;
    kefir_size_t colocated_bit_alignment = 0;
    REQUIRE_OK(amd64_sysv_bitfield_props(colocated_base, &colocated_bit_size, &colocated_bit_alignment));

    REQUIRE(bitwidth <= colocated_bit_size,
            KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Colocated bit-field exceeds storage unit width"));

    REQUIRE((payload->props.tail_offset & (colocated_bit_alignment - 1)) + bitwidth <= colocated_bit_size,
            KEFIR_SET_ERROR(KEFIR_OUT_OF_SPACE, "Colocated bit-field exceeds storage unit width"));

    bitfield->offset = payload->props.tail_offset - payload->props.head_offset;
    bitfield->width = bitwidth;

    payload->props.tail_offset += bitwidth;
    if (named) {
        typeentry->alignment = MAX(typeentry->alignment, colocated_bit_alignment / 8);
    }
    typeentry->param = payload->props.tail_offset - payload->props.head_offset;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_free(struct kefir_mem *mem, struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload, allocator->payload);

    KEFIR_FREE(mem, payload);
    allocator->has_bitfield_run = NULL;
    allocator->reset = NULL;
    allocator->next = NULL;
    allocator->next_colocated = NULL;
    allocator->free = NULL;
    allocator->payload = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_codegen_amd64_sysv_bitfield_allocator(struct kefir_mem *mem, struct kefir_ir_type *type,
                                                           struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to IR bitfield allocator"));

    struct bitfield_allocator_payload *payload = KEFIR_MALLOC(mem, sizeof(struct bitfield_allocator_payload));
    REQUIRE(payload != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AMD64 System V bitfield allocator payload"));

    payload->ir_type = type;
    payload->has_last_bitfield = false;
    payload->props.tail_offset = 0;
    payload->props.head_offset = 0;

    allocator->has_bitfield_run = amd64_sysv_bitfield_has_bitfield_run;
    allocator->reset = amd64_sysv_bitfield_reset;
    allocator->next = amd64_sysv_bitfield_next;
    allocator->next_colocated = amd64_sysv_bitfield_next_colocated;
    allocator->free = amd64_sysv_bitfield_free;
    allocator->payload = payload;
    return KEFIR_OK;
}
