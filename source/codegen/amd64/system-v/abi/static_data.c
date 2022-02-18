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

#include <string.h>
#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/vector.h"
#include "kefir/ir/builtins.h"

struct static_data_param {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_ir_data *data;
    struct kefir_vector layout;
    struct kefir_ir_type_visitor *visitor;
    kefir_size_t slot;
    kefir_size_t offset;
    struct kefir_ir_data_map_iterator data_map_iter;
};

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type, kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Encountered not supported type code while traversing IR type");
}

static kefir_result_t align_offset(struct kefir_amd64_sysv_data_layout *layout, struct static_data_param *param) {

    kefir_size_t new_offset = kefir_codegen_pad_aligned(param->offset, layout->alignment);
    if (new_offset > param->offset) {
        ASMGEN_MULRAW(&param->codegen->asmgen, new_offset - param->offset, KEFIR_AMD64_BYTE);
        ASMGEN_ARG0(&param->codegen->asmgen, "0");
        param->offset = new_offset;
    }
    return KEFIR_OK;
}

static kefir_result_t trailing_padding(kefir_size_t start_offset, struct kefir_amd64_sysv_data_layout *layout,
                                       struct static_data_param *param) {
    kefir_size_t end_offset = start_offset + layout->size;
    if (end_offset > param->offset) {
        ASMGEN_MULRAW(&param->codegen->asmgen, end_offset - param->offset, KEFIR_AMD64_BYTE);
        ASMGEN_ARG0(&param->codegen->asmgen, "0");
        param->offset = end_offset;
    } else {
        REQUIRE(end_offset == param->offset,
                KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Actual object size exceeds expected"));
    }
    return KEFIR_OK;
}

static kefir_result_t integral_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                           const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    const struct kefir_ir_data_value *entry;
    REQUIRE_OK(kefir_ir_data_value_at(param->data, param->slot++, &entry));

    kefir_int64_t value = 0;
    switch (entry->type) {
        case KEFIR_IR_DATA_VALUE_UNDEFINED:
            break;

        case KEFIR_IR_DATA_VALUE_INTEGER:
            value = entry->value.integer;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected value of integral field");
    }

    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));
    if (typeentry->typecode != KEFIR_IR_TYPE_BITS) {
        REQUIRE_OK(align_offset(layout, param));
    }
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_BOOL:
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_INT8:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_BYTE);
            ASMGEN_ARG(&param->codegen->asmgen, "0x%02x", (kefir_uint8_t) value);
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_WORD);
            ASMGEN_ARG(&param->codegen->asmgen, "0x%04x", (kefir_uint16_t) value);
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_DOUBLE);
            ASMGEN_ARG(&param->codegen->asmgen, "0x%08x", (kefir_uint32_t) value);
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_INT64:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG(&param->codegen->asmgen, "0x%016lx", value);
            break;

        case KEFIR_IR_TYPE_BITS: {
            kefir_size_t bits = typeentry->param;
            kefir_size_t bytes = bits / 8;
            if (bits % 8 != 0) {
                bytes += 1;
            }
            for (kefir_size_t i = 0; i < bytes; i++) {
                ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_BYTE);
                ASMGEN_ARG(&param->codegen->asmgen, "0x%02x", (value >> (i << 3)) & 0xff);
            }
        } break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                   KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-integral type");
    }
    param->offset += layout->size;
    return KEFIR_OK;
}

static kefir_result_t word_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                       const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    const struct kefir_ir_data_value *entry;
    REQUIRE_OK(kefir_ir_data_value_at(param->data, param->slot++, &entry));

    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));
    REQUIRE_OK(align_offset(layout, param));
    ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
    switch (entry->type) {
        case KEFIR_IR_DATA_VALUE_UNDEFINED:
            ASMGEN_ARG(&param->codegen->asmgen, "0x%016lx", 0);
            break;

        case KEFIR_IR_DATA_VALUE_INTEGER:
            ASMGEN_ARG(&param->codegen->asmgen, "0x%016lx", entry->value.integer);
            break;

        case KEFIR_IR_DATA_VALUE_POINTER:
            if (entry->value.pointer.offset > 0) {
                ASMGEN_ARG(&param->codegen->asmgen, "%s + " KEFIR_INT64_FMT, entry->value.pointer.reference,
                           entry->value.pointer.offset);
            } else if (entry->value.pointer.offset < 0) {
                ASMGEN_ARG(&param->codegen->asmgen, "%s " KEFIR_INT64_FMT, entry->value.pointer.reference,
                           entry->value.pointer.offset);
            } else {
                ASMGEN_ARG(&param->codegen->asmgen, "%s", entry->value.pointer.reference);
            }
            break;

        case KEFIR_IR_DATA_VALUE_STRING_POINTER:
            if (entry->value.pointer.offset > 0) {
                ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_STRING_LITERAL " + " KEFIR_INT64_FMT,
                           entry->value.string_ptr.id, entry->value.string_ptr.offset);
            } else if (entry->value.pointer.offset < 0) {
                ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_STRING_LITERAL " " KEFIR_INT64_FMT,
                           entry->value.string_ptr.id, entry->value.string_ptr.offset);
            } else {
                ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_STRING_LITERAL,
                           entry->value.string_ptr.id);
            }
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected value of word type field");
    }

    param->offset += layout->size;
    return KEFIR_OK;
}

static kefir_result_t float32_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    const struct kefir_ir_data_value *entry;
    REQUIRE_OK(kefir_ir_data_value_at(param->data, param->slot++, &entry));

    union {
        kefir_float32_t fp32;
        kefir_uint32_t uint32;
    } value = {.fp32 = 0.0f};
    switch (entry->type) {
        case KEFIR_IR_DATA_VALUE_UNDEFINED:
            break;

        case KEFIR_IR_DATA_VALUE_FLOAT32:
            value.fp32 = entry->value.float32;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected value of floating-point type field");
    }

    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));
    REQUIRE_OK(align_offset(layout, param));
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_FLOAT32:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_DOUBLE);
            ASMGEN_ARG(&param->codegen->asmgen, "0x%08" KEFIR_UINT32_XFMT, value.uint32);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                   KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-float32 type");
    }
    param->offset += layout->size;
    return KEFIR_OK;
}

static kefir_result_t float64_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    const struct kefir_ir_data_value *entry;
    REQUIRE_OK(kefir_ir_data_value_at(param->data, param->slot++, &entry));

    union {
        kefir_float64_t fp64;
        kefir_uint64_t uint64;
    } value = {.fp64 = 0.0};
    switch (entry->type) {
        case KEFIR_IR_DATA_VALUE_UNDEFINED:
            break;

        case KEFIR_IR_DATA_VALUE_FLOAT64:
            value.fp64 = entry->value.float64;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected value of floating-point type field");
    }

    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));
    REQUIRE_OK(align_offset(layout, param));
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_FLOAT64:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG(&param->codegen->asmgen, "0x%016" KEFIR_UINT64_XFMT, value.uint64);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                   KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-float type");
    }
    param->offset += layout->size;
    return KEFIR_OK;
}

static kefir_result_t long_double_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                              const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    const struct kefir_ir_data_value *entry;
    REQUIRE_OK(kefir_ir_data_value_at(param->data, param->slot++, &entry));

    union {
        kefir_long_double_t long_double;
        kefir_uint64_t uint64[2];
    } value = {.uint64 = {0, 0}};
    switch (entry->type) {
        case KEFIR_IR_DATA_VALUE_UNDEFINED:
            break;

        case KEFIR_IR_DATA_VALUE_LONG_DOUBLE:
            value.long_double = entry->value.long_double;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected value of long double type field");
    }

    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));
    REQUIRE_OK(align_offset(layout, param));
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_LONG_DOUBLE:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG(&param->codegen->asmgen, "0x%016" KEFIR_UINT64_XFMT, value.uint64[0]);
            ASMGEN_ARG(&param->codegen->asmgen, "0x%016" KEFIR_UINT64_XFMT, value.uint64[1]);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                   KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-long double type");
    }
    param->offset += layout->size;
    return KEFIR_OK;
}

static kefir_result_t struct_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                         const struct kefir_ir_typeentry *typeentry, void *payload) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));
    param->slot++;

    REQUIRE_OK(align_offset(layout, param));
    kefir_size_t start_offset = param->offset;
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, payload, index + 1, typeentry->param));

    REQUIRE_OK(trailing_padding(start_offset, layout, param));
    return KEFIR_OK;
}

static kefir_result_t union_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry, void *payload) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));
    param->slot++;

    REQUIRE_OK(align_offset(layout, param));
    kefir_size_t start_offset = param->offset;

    kefir_size_t subindex = index + 1;
    kefir_bool_t defined = false;
    for (kefir_size_t i = 0; i < (kefir_size_t) typeentry->param; i++) {
        const struct kefir_ir_data_value *subentry;
        REQUIRE_OK(kefir_ir_data_value_at(param->data, param->slot, &subentry));
        if (!defined && subentry->defined) {
            REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, payload, subindex, 1));
            defined = true;
        } else {
            param->slot += kefir_ir_type_node_slots(type, subindex);
        }
        subindex += kefir_ir_type_node_total_length(type, subindex);
    }

    REQUIRE_OK(trailing_padding(start_offset, layout, param));
    return KEFIR_OK;
}

static kefir_result_t dump_binary(struct static_data_param *param, const char *raw, kefir_size_t length) {
    ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_BYTE);
    for (kefir_size_t i = 0; i < length; i++) {
        ASMGEN_ARG(&param->codegen->asmgen, "0x%02x", (kefir_uint8_t) raw[i]);
    }
    param->offset += length;
    return KEFIR_OK;
}

static kefir_result_t array_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry, void *payload) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    const struct kefir_ir_data_value *entry;
    REQUIRE_OK(kefir_ir_data_value_at(param->data, param->slot++, &entry));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));

    REQUIRE_OK(align_offset(layout, param));
    kefir_size_t start_offset = param->offset;

    kefir_size_t array_element_slots = kefir_ir_type_node_slots(type, index + 1);
    kefir_size_t array_content_slots = kefir_ir_type_node_slots(type, index) - 1;

    switch (entry->type) {
        case KEFIR_IR_DATA_VALUE_UNDEFINED:
        case KEFIR_IR_DATA_VALUE_AGGREGATE: {
            kefir_size_t array_end_slot = param->slot + array_content_slots;
            ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, array_element_layout,
                             kefir_vector_at(&param->layout, index + 1));
            for (kefir_size_t i = 0; i < (kefir_size_t) typeentry->param; i++) {
                REQUIRE_OK(kefir_ir_data_map_skip_to(param->data, &param->data_map_iter, param->slot));
                if (!param->data_map_iter.has_mapped_values) {
                    param->slot = array_end_slot;
                    break;
                } else if (param->slot < param->data_map_iter.next_mapped_slot) {
                    const kefir_size_t missing_slots =
                        MIN(param->data_map_iter.next_mapped_slot, array_end_slot) - param->slot;
                    const kefir_size_t missing_array_elements = missing_slots / array_element_slots;
                    if (missing_array_elements > 0) {
                        i += missing_array_elements - 1;
                        param->slot += missing_array_elements * array_element_slots;
                        kefir_size_t zero_count = missing_array_elements * array_element_layout->size;
                        param->offset += zero_count;
                        ASMGEN_MULRAW(&param->codegen->asmgen, zero_count, KEFIR_AMD64_BYTE);
                        ASMGEN_ARG0(&param->codegen->asmgen, "0");
                        continue;
                    }
                }

                REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, payload, index + 1, 1));
            }
        } break;

        case KEFIR_IR_DATA_VALUE_STRING:
            REQUIRE_OK(dump_binary(param, entry->value.raw.data, entry->value.raw.length));
            param->slot += array_content_slots;
            break;

        case KEFIR_IR_DATA_VALUE_RAW: {
            ASSIGN_DECL_CAST(const char *, raw, entry->value.raw.data);
            REQUIRE_OK(dump_binary(param, raw, entry->value.raw.length));
            param->slot += array_content_slots;
        } break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected array data type");
    }

    REQUIRE_OK(trailing_padding(start_offset, layout, param));
    return KEFIR_OK;
}

static kefir_result_t memory_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                         const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    const struct kefir_ir_data_value *entry;
    REQUIRE_OK(kefir_ir_data_value_at(param->data, param->slot++, &entry));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));

    REQUIRE_OK(align_offset(layout, param));
    kefir_size_t start_offset = param->offset;

    switch (entry->type) {
        case KEFIR_IR_DATA_VALUE_UNDEFINED:
            break;

        case KEFIR_IR_DATA_VALUE_STRING:
            REQUIRE_OK(dump_binary(param, entry->value.raw.data, entry->value.raw.length));
            break;

        case KEFIR_IR_DATA_VALUE_RAW: {
            ASSIGN_DECL_CAST(const char *, raw, entry->value.raw.data);
            REQUIRE_OK(dump_binary(param, raw, entry->value.raw.length));
        } break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected memory data type");
    }

    REQUIRE_OK(trailing_padding(start_offset, layout, param));
    return KEFIR_OK;
}

static kefir_result_t pad_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                      const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));
    param->slot++;

    REQUIRE_OK(align_offset(layout, param));
    ASMGEN_MULRAW(&param->codegen->asmgen, layout->size, KEFIR_AMD64_BYTE);
    ASMGEN_ARG0(&param->codegen->asmgen, "0");
    param->offset += layout->size;
    return KEFIR_OK;
}

static kefir_result_t builtin_static_data(const struct kefir_ir_type *type, kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct static_data_param *, param, payload);
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&param->layout, index));
    param->slot++;

    switch ((kefir_ir_builtin_type_t) typeentry->param) {
        case KEFIR_IR_TYPE_BUILTIN_VARARG:
            REQUIRE_OK(align_offset(layout, param));
            ASMGEN_MULRAW(&param->codegen->asmgen, layout->size, KEFIR_AMD64_BYTE);
            ASMGEN_ARG0(&param->codegen->asmgen, "0");
            param->offset += layout->size;
            break;

        case KEFIR_IR_TYPE_BUILTIN_COUNT:
            return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unexpected built-in type");
    }
    return KEFIR_OK;
}

struct type_properties {
    kefir_size_t size;
    kefir_size_t alignment;
    struct kefir_vector *layout;
};

static kefir_result_t calculate_type_properties_visitor(const struct kefir_ir_type *type, kefir_size_t index,
                                                        const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(typeentry);

    ASSIGN_DECL_CAST(struct type_properties *, props, payload);
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(props->layout, index));
    props->size = kefir_codegen_pad_aligned(props->size, layout->alignment);
    props->size += layout->size;
    props->alignment = MAX(props->alignment, layout->alignment);
    return KEFIR_OK;
}

static kefir_result_t calculate_type_properties(const struct kefir_ir_type *type, struct kefir_vector *layout,
                                                kefir_size_t *size, kefir_size_t *alignment) {
    struct type_properties props = {.size = 0, .alignment = 0, .layout = layout};
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, calculate_type_properties_visitor));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, &visitor, (void *) &props, 0, kefir_ir_type_nodes(type)));
    *size = props.size;
    *alignment = props.alignment;
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_static_data(struct kefir_mem *mem, struct kefir_codegen_amd64 *codegen,
                                            const struct kefir_ir_data *data, const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AMD64 code generator"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR data"));
    REQUIRE(data->finalized, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected finalized IR data"));

    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, integral_static_data);
    visitor.visit[KEFIR_IR_TYPE_WORD] = word_static_data;
    visitor.visit[KEFIR_IR_TYPE_FLOAT32] = float32_static_data;
    visitor.visit[KEFIR_IR_TYPE_FLOAT64] = float64_static_data;
    visitor.visit[KEFIR_IR_TYPE_LONG_DOUBLE] = long_double_static_data;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = array_static_data;
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = struct_static_data;
    visitor.visit[KEFIR_IR_TYPE_UNION] = union_static_data;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = memory_static_data;
    visitor.visit[KEFIR_IR_TYPE_PAD] = pad_static_data;
    visitor.visit[KEFIR_IR_TYPE_BUILTIN] = builtin_static_data;

    struct static_data_param param = {.codegen = codegen, .data = data, .visitor = &visitor, .slot = 0, .offset = 0};
    REQUIRE_OK(kefir_ir_data_map_iter(data, &param.data_map_iter));
    REQUIRE_OK(kefir_amd64_sysv_type_layout(data->type, mem, &param.layout));

    kefir_size_t total_size, total_alignment;
    kefir_result_t res = calculate_type_properties(data->type, &param.layout, &total_size, &total_alignment);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, &param.layout);
        return res;
    });
    if (identifier != NULL) {
        if (total_alignment > 1) {
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ALIGN);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_SIZE_FMT, total_alignment);
        }
        ASMGEN_LABEL(&codegen->asmgen, "%s", identifier);
    }

    res = kefir_ir_type_visitor_list_nodes(data->type, &visitor, (void *) &param, 0, kefir_ir_type_nodes(data->type));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, &param.layout);
        return res;
    });

    if (param.offset < total_size) {
        ASMGEN_MULRAW(&codegen->asmgen, total_size - param.offset, KEFIR_AMD64_BYTE);
        ASMGEN_ARG0(&codegen->asmgen, "0");
    }

    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    REQUIRE_OK(kefir_vector_free(mem, &param.layout));
    return KEFIR_OK;
}
