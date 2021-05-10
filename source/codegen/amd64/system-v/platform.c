#include "kefir/codegen/amd64/system-v/platform.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t amd64_sysv_get_type(struct kefir_mem *mem,
                                        struct kefir_ir_target_platform *platform,
                                        const struct kefir_ir_type *ir_type,
                                        kefir_ir_target_platform_opaque_type_t *type_ptr) {
    UNUSED(platform);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(ir_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(type_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR target platform type pointer"));

    struct kefir_codegen_amd64_sysv_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_codegen_amd64_sysv_type));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AMD64 SysV platform IR type"));
    type->ir_type = ir_type;
    kefir_result_t res = kefir_amd64_sysv_type_layout(ir_type, mem, &type->layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type);
        return res;
    });

    *type_ptr = type;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_free_type(struct kefir_mem *mem,
                                         struct kefir_ir_target_platform *platform,
                                         kefir_ir_target_platform_opaque_type_t platform_type) {
    UNUSED(platform);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(platform_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform type"));

    ASSIGN_DECL_CAST(struct kefir_codegen_amd64_sysv_type *, type,
        platform_type);
    REQUIRE_OK(kefir_vector_free(mem, &type->layout));
    type->ir_type = NULL;
    KEFIR_FREE(mem, type);
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_type_info(struct kefir_mem *mem,
                                         struct kefir_ir_target_platform *platform,
                                         kefir_ir_target_platform_opaque_type_t platform_type,
                                         kefir_size_t index,
                                         struct kefir_ir_target_type_info *type_info) {
    UNUSED(platform);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(platform_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform type"));
    REQUIRE(type_info != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid target type info"));

    ASSIGN_DECL_CAST(struct kefir_codegen_amd64_sysv_type *, type,
        platform_type);
    REQUIRE(index < kefir_ir_type_total_length(type->ir_type),
        KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Specified index is out of bounds of IR type"));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, data_layout,
        kefir_vector_at(&type->layout, index));
    type_info->size = data_layout->size;
    type_info->alignment = data_layout->alignment;
    type_info->aligned = data_layout->aligned;
    type_info->relative_offset = data_layout->relative_offset;
    return KEFIR_OK;
}

struct bitfield_allocator_payload {
    struct kefir_ir_type *ir_type;
    kefir_bool_t has_last_bitfield;
    struct kefir_ir_bitfield last_bitfield;
    kefir_size_t max_alignment;
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

static kefir_result_t type_props(struct kefir_mem *mem,
                               struct kefir_ir_type *type,
                               kefir_size_t index,
                               kefir_size_t *offset,
                               kefir_size_t *alignment) {
    struct kefir_vector layout;
    REQUIRE_OK(kefir_amd64_sysv_type_layout(type, mem, &layout));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, entry_layout,
        kefir_vector_at(&layout, index));
    REQUIRE_ELSE(entry_layout != NULL, {
        kefir_vector_free(mem, &layout);
        return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected type layout at index");
    });
    ASSIGN_PTR(offset, entry_layout->relative_offset);
    ASSIGN_PTR(alignment, entry_layout->alignment);
    REQUIRE_OK(kefir_vector_free(mem, &layout));
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_finalize(struct kefir_mem *mem,
                                                 struct kefir_ir_bitfield_allocator *allocator,
                                                 struct kefir_irbuilder_type *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type builder"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload,
        allocator->payload);

    if (payload->max_alignment > 0) {
        REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_ALIGN, 0, payload->max_alignment));
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

static kefir_result_t amd64_sysv_bitfield_props(const struct kefir_ir_typeentry *typeentry,
                                              kefir_uint8_t *width,
                                              kefir_size_t *alignment) {
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_BOOL:
        case KEFIR_IR_TYPE_INT8:
            ASSIGN_PTR(width, 8);
            ASSIGN_PTR(alignment, 1);
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            ASSIGN_PTR(width, 16);
            ASSIGN_PTR(alignment, 2);
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            ASSIGN_PTR(width, 32);
            ASSIGN_PTR(alignment, 4);
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_WORD:
        case KEFIR_IR_TYPE_INT64:
            ASSIGN_PTR(width, 64);
            ASSIGN_PTR(alignment, 8);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected bit-field type");
    }

    if (typeentry->alignment != 0) {
        ASSIGN_PTR(alignment, typeentry->alignment);
    }
    return KEFIR_OK;
}

static kefir_result_t bitfield_typecode(kefir_uint8_t width,
                                      struct kefir_ir_typeentry *typeentry,
                                      kefir_size_t max_alignment,
                                      kefir_size_t *alignment,
                                      kefir_uint8_t *rounded_width) {
    kefir_size_t current_alignment = 0;
    if (width <= 8) {
        typeentry->typecode = KEFIR_IR_TYPE_CHAR;
        current_alignment = 1;
        ASSIGN_PTR(rounded_width, 8);
    } else if (width <= 16) {
        typeentry->typecode = KEFIR_IR_TYPE_SHORT;
        current_alignment = 2;
        ASSIGN_PTR(rounded_width, 16);
    } else if (width <= 32) {
        typeentry->typecode = KEFIR_IR_TYPE_INT;
        current_alignment = 4;
        ASSIGN_PTR(rounded_width, 32);
    } else if (width <= 64) {
        typeentry->typecode = KEFIR_IR_TYPE_LONG;
        current_alignment = 8;
        ASSIGN_PTR(rounded_width, 64);
    } else {
        return KEFIR_SET_ERROR(KEFIR_OUT_OF_SPACE, "Requested bit-field width exceeds storage type width");
    }
    
    kefir_size_t final_alignment = MIN(current_alignment, max_alignment);
    ASSIGN_PTR(alignment, final_alignment);
    typeentry->alignment = final_alignment != current_alignment ? final_alignment : 0;
    typeentry->param = 0;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_next(struct kefir_mem *mem,
                                             struct kefir_ir_bitfield_allocator *allocator,
                                             kefir_size_t index,
                                             struct kefir_ir_typeentry *typeentry,
                                             uint8_t bitwidth,
                                             struct kefir_ir_bitfield *bitfield) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type entry"));
    REQUIRE(bitwidth != 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero bit-field width"));
    REQUIRE(bitfield != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR bitfield"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload,
        allocator->payload);

    kefir_uint8_t type_width = 0;
    kefir_size_t type_alignment = 0;
    REQUIRE_OK(amd64_sysv_bitfield_props(typeentry, &type_width, &type_alignment));
    REQUIRE(bitwidth <= type_width,
        KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Requested bit-field width exceeds storage type width"));

    kefir_uint8_t rounded_width;
    REQUIRE_OK(bitfield_typecode(bitwidth, typeentry, type_alignment, NULL, &rounded_width));

    payload->has_last_bitfield = true;
    payload->last_bitfield.location = index;
    payload->last_bitfield.offset = bitwidth;
    payload->last_bitfield.width = rounded_width;
    payload->max_alignment = MAX(type_alignment, payload->max_alignment);

    bitfield->location = payload->last_bitfield.location;
    bitfield->offset = 0;
    bitfield->width = bitwidth;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_next_colocated(struct kefir_mem *mem,
                                                       struct kefir_ir_bitfield_allocator *allocator,
                                                       const struct kefir_ir_typeentry *colocated,
                                                       uint8_t bitwidth,
                                                       struct kefir_ir_bitfield *bitfield) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    REQUIRE(colocated != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type entry"));
    REQUIRE(bitwidth != 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero bit-field width"));
    REQUIRE(bitfield != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR bitfield"));
    ASSIGN_DECL_CAST(struct bitfield_allocator_payload *, payload,
        allocator->payload);
    REQUIRE(payload->has_last_bitfield,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to colocate a bit-field"));

    struct kefir_ir_typeentry *original = kefir_ir_type_at(payload->ir_type, payload->last_bitfield.location);
    kefir_uint8_t original_width = 0;
    kefir_size_t original_alignment = 0;
    REQUIRE_OK(amd64_sysv_bitfield_props(original, &original_width, &original_alignment));

    kefir_uint8_t type_width = 0;
    kefir_size_t type_alignment = 0;
    REQUIRE_OK(amd64_sysv_bitfield_props(colocated, &type_width, &type_alignment));
    REQUIRE(bitwidth <= type_width,
        KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Requested bit-field width exceeds storage type width"));
    
    kefir_size_t required_width = payload->last_bitfield.offset + bitwidth;
    kefir_size_t max_width = MAX(type_width, payload->last_bitfield.width);

    kefir_bool_t pad_offset = required_width > max_width;
    // REQUIRE(required_width <= max_width,
    //     KEFIR_SET_ERROR(KEFIR_OUT_OF_SPACE, "Requested bit-field exceeds storage unit width"));
    
    kefir_size_t new_alignment = 0;
    kefir_uint8_t new_width = 0;
    struct kefir_ir_typeentry new_typeentry;
    REQUIRE_OK(bitfield_typecode(required_width, &new_typeentry, MAX(original_alignment, type_alignment), &new_alignment, &new_width));

    kefir_size_t last_bitfield_offset = 0;
    REQUIRE_OK(type_props(mem, payload->ir_type, payload->last_bitfield.location, &last_bitfield_offset, NULL));
    REQUIRE(last_bitfield_offset % new_alignment == 0,
        KEFIR_SET_ERROR(KEFIR_OUT_OF_SPACE, "Unable to allocate requested storage unit due to alignment requirements"));

    if (pad_offset) {
        payload->last_bitfield.offset = max_width;
    }
    *original = new_typeentry;
    bitfield->offset = payload->last_bitfield.offset;
    bitfield->width = bitwidth;

    payload->last_bitfield.offset += bitwidth;
    payload->last_bitfield.width = new_width;
    payload->max_alignment = MAX(type_alignment, payload->max_alignment);
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
    allocator->finalize = NULL;
    allocator->reset = NULL;
    allocator->next = NULL;
    allocator->next_colocated = NULL;
    allocator->free = NULL;
    allocator->payload = NULL;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_allocator(struct kefir_mem *mem,
                                                  struct kefir_ir_target_platform *platform,
                                                  struct kefir_ir_type *type,
                                                  struct kefir_ir_bitfield_allocator *allocator) {
    UNUSED(platform);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to IR bitfield allocator"));

    struct bitfield_allocator_payload *payload = KEFIR_MALLOC(mem, sizeof(struct bitfield_allocator_payload));
    REQUIRE(payload != NULL,
        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AMD64 System V bitfield allocator payload"));

    payload->ir_type = type;
    payload->has_last_bitfield = false;
    payload->last_bitfield = (struct kefir_ir_bitfield){0};
    payload->max_alignment = 0;

    allocator->get_last_bitfield = amd64_sysv_bitfield_get_last_bitfield;
    allocator->finalize = amd64_sysv_bitfield_finalize;
    allocator->reset = amd64_sysv_bitfield_reset;
    allocator->next = amd64_sysv_bitfield_next;
    allocator->next_colocated = amd64_sysv_bitfield_next_colocated;
    allocator->free = amd64_sysv_bitfield_free;
    allocator->payload = payload;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_free(struct kefir_mem *mem,
                                    struct kefir_ir_target_platform *platform) {
    UNUSED(mem);
    REQUIRE(platform != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid target platform"));
    platform->type_info = NULL;
    platform->free = NULL;
    platform->payload = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_codegen_amd64_sysv_target_platform(struct kefir_ir_target_platform *platform) {
    REQUIRE(platform != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation platform pointer"));
    platform->get_type = amd64_sysv_get_type;
    platform->free_type = amd64_sysv_free_type;
    platform->type_info = amd64_sysv_type_info;
    platform->bitfield_allocator = amd64_sysv_bitfield_allocator;
    platform->free = amd64_sysv_free;
    platform->payload = NULL;
    return KEFIR_OK;
}