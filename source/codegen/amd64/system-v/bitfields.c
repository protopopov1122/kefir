#include "kefir/codegen/amd64/system-v/bitfields.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct bitfield_allocator_payload {
    struct kefir_ir_type *ir_type;
    kefir_bool_t has_last_bitfield;
    struct kefir_ir_bitfield last_bitfield;
    struct {
        kefir_size_t unit_start;
        kefir_size_t initial_offset;
    } props;
};

static kefir_result_t amd64_sysv_bitfield_get_last_bitfield(struct kefir_ir_bitfield_allocator *allocator,
                                                          const struct kefir_ir_bitfield **bitfield_p) {
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    REQUIRE(bitfield_p != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR bitfield pointer"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload,
        allocator->payload);
    
    if (payload->has_last_bitfield) {
        *bitfield_p = &payload->last_bitfield;
    } else {
        *bitfield_p = NULL;
    }
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_reset(struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload,
        allocator->payload);
    payload->has_last_bitfield = false;
    payload->last_bitfield = (struct kefir_ir_bitfield){0};
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_props(kefir_ir_typecode_t typecode,
                                              kefir_size_t *size,
                                              kefir_size_t *alignment) {
    switch (typecode) {
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_BOOL:
        case KEFIR_IR_TYPE_INT8:
            ASSIGN_PTR(size, 1);
            ASSIGN_PTR(alignment, 1);
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            ASSIGN_PTR(size, 2);
            ASSIGN_PTR(alignment, 2);
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            ASSIGN_PTR(size, 4);
            ASSIGN_PTR(alignment, 4);
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_WORD:
        case KEFIR_IR_TYPE_INT64:
            ASSIGN_PTR(size, 8);
            ASSIGN_PTR(alignment, 8);
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

static kefir_result_t visit_struct_layout(const struct kefir_ir_type *type,
                                        kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry,
                                        void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct struct_layout_visitor *, visitor_payload,
        payload);
    
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
        kefir_vector_at(&visitor_payload->layout, index));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Failed to fetch IR type layout"));
    *visitor_payload->offset = MAX(*visitor_payload->offset, layout->relative_offset + layout->size);
    return KEFIR_OK;
}

static kefir_result_t struct_current_offset(struct kefir_mem *mem,
                                          const struct kefir_ir_type *type,
                                          kefir_size_t struct_index,
                                          kefir_size_t *offset) {
    const struct kefir_ir_typeentry *struct_typeentry = kefir_ir_type_at(type, struct_index);
    REQUIRE(struct_typeentry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR structure type index"));
    if (struct_typeentry->param > 0) {
        struct kefir_ir_type_visitor visitor;
        struct struct_layout_visitor payload = {
            .offset = offset
        };
        REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, &visit_struct_layout));
        REQUIRE_OK(kefir_amd64_sysv_type_layout_of(mem, type, struct_index, 1, &payload.layout));
        kefir_result_t res = kefir_ir_type_visitor_list_nodes(type, &visitor, &payload, struct_index + 1,
            struct_typeentry->param);
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

static kefir_result_t round_bitwidth(kefir_size_t bitwidth, kefir_uint8_t *result) {
    if (bitwidth <= 8) {
        ASSIGN_PTR(result, 8);
    } else if (bitwidth <= 16) {
        ASSIGN_PTR(result, 16);
    } else if (bitwidth <= 32) {
        ASSIGN_PTR(result, 32);
    } else if (bitwidth <= 40) {
        ASSIGN_PTR(result, 40);
    } else if (bitwidth <= 48) {
        ASSIGN_PTR(result, 48);
    } else if (bitwidth <= 56) {
        ASSIGN_PTR(result, 56);
    } else if (bitwidth <= 64) {
        ASSIGN_PTR(result, 64);
    } else {
        return KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Bit-field width exceeds storage unit width");
    }
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_next(struct kefir_mem *mem,
                                             struct kefir_ir_bitfield_allocator *allocator,
                                             kefir_size_t struct_index,
                                             kefir_size_t location,
                                             uint8_t bitwidth,
                                             struct kefir_ir_bitfield *bitfield) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    REQUIRE(bitwidth != 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero bit-field width"));
    REQUIRE(bitfield != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR bitfield"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload,
        allocator->payload);

    const struct kefir_ir_typeentry *struct_typeentry = kefir_ir_type_at(payload->ir_type, struct_index);
    REQUIRE(struct_typeentry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR structure type index"));

    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(payload->ir_type, location);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type entry"));
    kefir_ir_typecode_t base_typecode = typeentry->typecode;

    kefir_size_t current_offset = 0;
    REQUIRE_OK(struct_current_offset(mem, payload->ir_type, struct_index, &current_offset));

    kefir_size_t size = bitwidth / 8 + (bitwidth % 8 != 0 ? 1 : 0);

    kefir_size_t base_size = 0;
    kefir_size_t base_alignment = 0;
    REQUIRE_OK(amd64_sysv_bitfield_props(base_typecode, &base_size, &base_alignment));
    REQUIRE(bitwidth <= base_size * 8, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Bit-field width exceeds storage unit width"));

    kefir_size_t unit_end = current_offset + base_size;
    unit_end -= unit_end % base_alignment;

    kefir_size_t unit_start = 0;
    if (size <= unit_end - current_offset) {
        // Bit-field fits into current storage unit
        unit_start = unit_end - base_size;
    } else {
        // New storage unit shall be allocated
        unit_start = kefir_codegen_pad_aligned(current_offset, base_alignment);
    }
    REQUIRE_OK(round_bitwidth(bitwidth, &payload->last_bitfield.width));
    payload->last_bitfield.location = location;
    payload->last_bitfield.offset = bitwidth;
    payload->has_last_bitfield = true;

    payload->props.initial_offset = current_offset;
    payload->props.unit_start = unit_start;

    typeentry->typecode = KEFIR_IR_TYPE_BITS;
    typeentry->alignment = 0;
    typeentry->param = KEFIR_IR_BITS_PARAM(base_typecode, bitwidth, 0);

    bitfield->location = payload->last_bitfield.location;
    bitfield->offset = 0;
    bitfield->width = bitwidth;

    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_next_colocated(struct kefir_mem *mem,
                                                       struct kefir_ir_bitfield_allocator *allocator,
                                                       kefir_ir_typecode_t colocated_base,
                                                       uint8_t bitwidth,
                                                       struct kefir_ir_bitfield *bitfield) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    REQUIRE(bitwidth != 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero bit-field width"));
    REQUIRE(bitfield != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR bitfield"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload,
        allocator->payload);
    REQUIRE(payload->has_last_bitfield,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to colocate a bit-field"));

    struct kefir_ir_typeentry *original = kefir_ir_type_at(payload->ir_type, payload->last_bitfield.location);
    REQUIRE(original != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to fetch original bit-field"));
    
    kefir_ir_typecode_t base_typecode;
    kefir_size_t pad = 0;
    KEFIR_IR_BITS_PARAM_GET(original->param, &base_typecode, NULL, &pad);

    kefir_size_t original_size = 0;
    kefir_size_t colocated_size = 0;
    kefir_size_t colocated_alignment = 0;
    REQUIRE_OK(amd64_sysv_bitfield_props(base_typecode, &original_size, NULL));
    REQUIRE_OK(amd64_sysv_bitfield_props(colocated_base, &colocated_size, &colocated_alignment));

    REQUIRE(bitwidth <= colocated_size * 8,
        KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Colocated bit-field exceeds storage unit width"));
    
    kefir_size_t current_offset = payload->props.unit_start;
    kefir_size_t unit_end = current_offset + colocated_size;
    unit_end -= unit_end % colocated_size;

    kefir_size_t unit_start = unit_end - colocated_size;
    REQUIRE(unit_start % colocated_alignment == 0, 
        KEFIR_SET_ERROR(KEFIR_OUT_OF_SPACE, "Unable to colocate bit-field in current storage unit"));

    kefir_size_t unit_pad = pad;
    if (unit_start < payload->props.unit_start) {
        unit_pad += (payload->props.unit_start - MAX(payload->props.initial_offset, unit_start)) * 8;
    }

    kefir_size_t available_width = (unit_end - MAX(unit_start, payload->props.initial_offset)) * 8;

    kefir_ir_typecode_t new_base = colocated_size > original_size ? colocated_base : base_typecode;
    if (available_width < payload->last_bitfield.width) {
        REQUIRE(payload->last_bitfield.offset + bitwidth <= payload->last_bitfield.width,
            KEFIR_SET_ERROR(KEFIR_OUT_OF_SPACE, "Colocated bit-field exceeds storage unit width"));
        bitfield->location = payload->last_bitfield.location;
        bitfield->offset = payload->last_bitfield.offset;
        bitfield->width = bitwidth;

        payload->last_bitfield.offset += bitwidth;
        original->param = KEFIR_IR_BITS_PARAM(new_base, payload->last_bitfield.offset, pad);
    } else {
        kefir_size_t total_bitwidth = unit_pad + payload->last_bitfield.offset + bitwidth;
        REQUIRE(total_bitwidth <= available_width,
            KEFIR_SET_ERROR(KEFIR_OUT_OF_SPACE, "Colocated bit-field exceeds storage unit width"));
        bitfield->location = payload->last_bitfield.location;
        bitfield->offset = payload->last_bitfield.offset;
        bitfield->width = bitwidth;

        kefir_uint8_t rounded_width = 0;
        REQUIRE_OK(round_bitwidth(payload->last_bitfield.offset + bitwidth, &rounded_width));
        payload->last_bitfield.width = MIN(rounded_width, available_width);
        payload->last_bitfield.offset += bitwidth;

        if (payload->props.initial_offset < payload->props.unit_start) {
            payload->props.unit_start = MIN(unit_start, payload->props.unit_start);
        }
        original->typecode = KEFIR_IR_TYPE_BITS;
        original->alignment = 0;
        original->param = KEFIR_IR_BITS_PARAM(new_base, payload->last_bitfield.offset, unit_pad);
    }
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_free(struct kefir_mem *mem,
                                             struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload,
        allocator->payload);

    KEFIR_FREE(mem, payload);
    allocator->get_last_bitfield = NULL;
    allocator->reset = NULL;
    allocator->next = NULL;
    allocator->next_colocated = NULL;
    allocator->free = NULL;
    allocator->payload = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_codegen_amd64_sysv_bitfield_allocator(struct kefir_mem *mem,
                                                       struct kefir_ir_type *type,
                                                       struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to IR bitfield allocator"));

    struct bitfield_allocator_payload *payload = KEFIR_MALLOC(mem, sizeof(struct bitfield_allocator_payload));
    REQUIRE(payload != NULL,
        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AMD64 System V bitfield allocator payload"));

    payload->ir_type = type;
    payload->has_last_bitfield = false;
    payload->last_bitfield = (struct kefir_ir_bitfield){0};
    payload->props.initial_offset = 0;
    payload->props.unit_start = 0;

    allocator->get_last_bitfield = amd64_sysv_bitfield_get_last_bitfield;
    allocator->reset = amd64_sysv_bitfield_reset;
    allocator->next = amd64_sysv_bitfield_next;
    allocator->next_colocated = amd64_sysv_bitfield_next_colocated;
    allocator->free = amd64_sysv_bitfield_free;
    allocator->payload = payload;
    return KEFIR_OK;
}