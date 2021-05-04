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

const struct kefir_ir_bitfield *amd64_sysv_bitfield_get_state(struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(allocator != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ir_bitfield *, payload,
            allocator->payload);
    REQUIRE(payload->width != 0, NULL);
    return payload;
}

static kefir_result_t amd64_sysv_bitfield_reset(struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    ASSIGN_DECL_CAST(struct kefir_ir_bitfield *, payload,
        allocator->payload);
    payload->width = 0;
    payload->offset = 0;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_init(struct kefir_ir_bitfield_allocator *allocator,
                                             kefir_ir_typecode_t typecode) {
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    ASSIGN_DECL_CAST(struct kefir_ir_bitfield *, payload,
        allocator->payload);
    switch (typecode) {
        case KEFIR_IR_TYPE_BOOL:
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_INT8:
            payload->width = 8;
            payload->offset = 0;
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            payload->width = 16;
            payload->offset = 0;
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            payload->width = 32;
            payload->offset = 0;
            break;
        
        case KEFIR_IR_TYPE_INT64:
        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_WORD:
            payload->width = 64;
            payload->offset = 0;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected bitfield type code");
    }
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_next(struct kefir_ir_bitfield_allocator *allocator,
                                             kefir_uint8_t bitfield_width,
                                             struct kefir_ir_bitfield *bitfield) {   
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    REQUIRE(bitfield_width != 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid bitfield width"));
    REQUIRE(bitfield != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to IR bitfield"));
    ASSIGN_DECL_CAST(struct kefir_ir_bitfield *, payload,
        allocator->payload);
    REQUIRE(payload->width != 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected IR bitfield allocator to be initialized"));

    REQUIRE(bitfield_width <= payload->width,
        KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Requested bitfield width exceeds underlying type width"));
    REQUIRE(payload->offset + bitfield_width <= payload->width,
        KEFIR_SET_ERROR(KEFIR_OUT_OF_SPACE, "Requested bitfield cannot be allocated in the current scalar"));
    
    bitfield->width = bitfield_width;
    bitfield->offset = payload->offset;
    payload->offset += bitfield_width;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_free(struct kefir_mem *mem,
                                             struct kefir_ir_bitfield_allocator *allocator) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR platform bitfield allocator"));
    ASSIGN_DECL_CAST(struct bitfield_allocator *, payload,
        allocator->payload);

    KEFIR_FREE(mem, payload);
    allocator->get_state = NULL;
    allocator->init = NULL;
    allocator->reset = NULL;
    allocator->next = NULL;
    allocator->free = NULL;
    allocator->payload = NULL;
    return KEFIR_OK;
}

static kefir_result_t amd64_sysv_bitfield_allocator(struct kefir_mem *mem,
                                                  struct kefir_ir_target_platform *platform,
                                                  struct kefir_ir_bitfield_allocator *allocator) {
    UNUSED(platform);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to IR bitfield allocator"));

    struct kefir_ir_bitfield *payload = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_bitfield));
    REQUIRE(payload != NULL,
        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AMD64 System V bitfield allocator payload"));
    payload->width = 0;
    payload->offset = 0;

    allocator->get_state = amd64_sysv_bitfield_get_state;
    allocator->reset = amd64_sysv_bitfield_reset;
    allocator->init = amd64_sysv_bitfield_init;
    allocator->next = amd64_sysv_bitfield_next;
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