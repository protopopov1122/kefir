#include <stdbool.h>
#include "kefir/core/error.h"
#include "kefir/codegen/amd64/system-v/abi_data.h"
#include "kefir/codegen/amd64/asmgen.h"

const char *KEFIR_AMD64_SYSV_INTEGER_REGISTERS[] = {
    KEFIR_AMD64_RDI,
    KEFIR_AMD64_RSI,
    KEFIR_AMD64_RDX,
    KEFIR_AMD64_RCX,
    KEFIR_AMD64_R8,
    KEFIR_AMD64_R9
};

kefir_size_t KEFIR_AMD64_SYSV_INTEGER_REGISTER_COUNT =
    sizeof(KEFIR_AMD64_SYSV_INTEGER_REGISTERS) / sizeof(KEFIR_AMD64_SYSV_INTEGER_REGISTERS[0]);

const char *KEFIR_AMD64_SYSV_SSE_REGISTERS[] = {
    KEFIR_AMD64_XMM0,
    KEFIR_AMD64_XMM1,
    KEFIR_AMD64_XMM2,
    KEFIR_AMD64_XMM3,
    KEFIR_AMD64_XMM4,
    KEFIR_AMD64_XMM5,
    KEFIR_AMD64_XMM6,
    KEFIR_AMD64_XMM7
};

kefir_size_t KEFIR_AMD64_SYSV_SSE_REGISTER_COUNT =
    sizeof(KEFIR_AMD64_SYSV_SSE_REGISTERS) / sizeof(KEFIR_AMD64_SYSV_SSE_REGISTERS[0]);

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Encountered not supported type code while traversing type");
}

// static kefir_result_t pad_size(kefir_size_t size, kefir_size_t alignment) {
//     const kefir_size_t padding = size % alignment;
//     if (padding != 0) {
//         size += alignment - padding;
//     }
//     return size;
// }

struct parameter_allocation_info {
    const struct kefir_vector *layout;
    struct kefir_vector *allocation;

    kefir_size_t integer_register;
    kefir_size_t sse_register;
    kefir_size_t sseup_register;
    kefir_size_t memory_offset;
};

static kefir_result_t allocate_integer(const struct kefir_ir_type *type,
                                     kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry,
                                     void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct parameter_allocation_info *info = (struct parameter_allocation_info *) payload;
    struct kefir_amd64_sysv_parameter_allocation *data =
        (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(info->allocation, index);
    if (info->integer_register == KEFIR_AMD64_SYSV_INTEGER_REGISTER_COUNT) {
        return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
            KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Available number of integral registers exceeded");
    }
    data->dataclass = KEFIR_AMD64_SYSV_PARAM_INTEGER;
    data->location = info->integer_register++;
    return KEFIR_OK;
}

static kefir_result_t allocate_sse(const struct kefir_ir_type *type,
                                 kefir_size_t index,
                                 const struct kefir_ir_typeentry *typeentry,
                                 void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct parameter_allocation_info *info = (struct parameter_allocation_info *) payload;
    struct kefir_amd64_sysv_parameter_allocation *data =
        (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(info->allocation, index);
    if (info->sse_register == KEFIR_AMD64_SYSV_SSE_REGISTER_COUNT) {
        return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
            KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Available number of SSE register exceeded");
    }
    data->dataclass = KEFIR_AMD64_SYSV_PARAM_SSE;
    data->location = info->sse_register++;
    return KEFIR_OK;
}

static kefir_result_t allocate_data(const struct kefir_ir_type *type,
                                  const struct kefir_vector *layout,
                                  struct kefir_vector *allocation) {
    const kefir_size_t length = kefir_ir_type_length(type);
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    struct parameter_allocation_info info = {
        .layout = layout,
        .allocation = allocation,
        .integer_register = 0,
        .sse_register = 0,
        .sseup_register = 0,
        .memory_offset = 0
    };
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, allocate_integer);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, allocate_sse);
    return kefir_ir_type_visitor_list_subtrees(type, &visitor, (void *) &info, 0, length);
}

kefir_result_t kefir_amd64_sysv_type_param_allocation(const struct kefir_ir_type *type,
                                          struct kefir_mem *mem,
                                          const struct kefir_vector *layout,
                                          struct kefir_vector *allocation) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid layout vector"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid allocation vector"));
    const kefir_size_t length = kefir_ir_type_length(type);
    REQUIRE_OK(kefir_vector_alloc(mem,
                                sizeof(struct kefir_amd64_sysv_parameter_allocation),
                                length,
                                allocation));
    kefir_result_t res = kefir_vector_extend(allocation, length);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, allocation);
        return res;
    });
    res = allocate_data(type, layout, allocation);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, allocation);
        return res;
    });
    return KEFIR_OK;
}