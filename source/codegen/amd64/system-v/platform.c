#include "kefir/codegen/amd64/system-v/platform.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/amd64/system-v/bitfields.h"
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

static kefir_result_t amd64_sysv_bitfield_allocator(struct kefir_mem *mem,
                                                  struct kefir_ir_target_platform *platform,
                                                  struct kefir_ir_type *type,
                                                  struct kefir_ir_bitfield_allocator *allocator) {
    UNUSED(platform);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(allocator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to IR bitfield allocator"));

    REQUIRE_OK(kefir_codegen_amd64_sysv_bitfield_allocator(mem, type, allocator));
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