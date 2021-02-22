#include "kefir/codegen/amd64/system-v/platform.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t amd64_sysv_type_info(struct kefir_mem *mem,
                                         struct kefir_ir_target_platform *platform,
                                         const struct kefir_ir_type *type,
                                         struct kefir_ir_target_type_info *type_info) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(platform != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid target platform"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(kefir_ir_type_total_length(type) > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-empty IR type"));
    REQUIRE(type_info != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid target type info"));
    struct kefir_vector layout;
    REQUIRE_OK(kefir_amd64_sysv_type_layout(type, mem, &layout));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, data_layout,
        kefir_vector_at(&layout, 0));
    type_info->size = data_layout->size;
    type_info->alignment = data_layout->alignment;
    type_info->aligned = data_layout->aligned;
    REQUIRE_OK(kefir_vector_free(mem, &layout));
    return KEFIR_OK;
}
kefir_result_t amd64_sysv_free(struct kefir_mem *mem,
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
    platform->type_info = amd64_sysv_type_info;
    platform->free = amd64_sysv_free;
    platform->payload = NULL;
    return KEFIR_OK;
}