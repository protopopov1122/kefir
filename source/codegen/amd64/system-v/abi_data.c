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

struct compound_type_layout {
    struct kefir_ir_type_visitor *visitor;
    struct kefir_vector *vector;
    kefir_size_t offset;
    kefir_size_t max_alignment;
    kefir_size_t max_size;
    bool aggregate;
    bool aligned;
};

static kefir_result_t pad_size(kefir_size_t size, kefir_size_t alignment) {
    const kefir_size_t padding = size % alignment;
    if (padding != 0) {
        size += alignment - padding;
    }
    return size;
}

static kefir_result_t update_compound_type_layout(struct compound_type_layout *compound_type_layout,
                                       struct kefir_amd64_sysv_data_layout *data,
                                       const struct kefir_ir_typeentry *typeentry) {
    if (typeentry->alignment != 0) {
        data->aligned = typeentry->alignment >= data->alignment;
        data->alignment = typeentry->alignment;
    }
    if (compound_type_layout->aggregate) {
        compound_type_layout->offset = pad_size(compound_type_layout->offset, data->alignment);
    }
    compound_type_layout->max_alignment = MAX(compound_type_layout->max_alignment, data->alignment);
    compound_type_layout->max_size = MAX(compound_type_layout->max_size, data->size);
    if (compound_type_layout->aligned && !data->aligned) {
        compound_type_layout->aligned = false;
    }
    data->relative_offset = compound_type_layout->offset;
    if (compound_type_layout->aggregate) {
        compound_type_layout->offset += data->size;
    }
    return KEFIR_OK;
}

static kefir_result_t calculate_integer_layout(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               const struct kefir_ir_typeentry *typeentry,
                               void *payload) {
    UNUSED(type);
    struct compound_type_layout *compound_type_layout = (struct compound_type_layout *) payload;
    struct kefir_amd64_sysv_data_layout *data =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(compound_type_layout->vector, index);
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_INT8:
            data->size = 1;
            data->alignment = 1;
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            data->size = 2;
            data->alignment = 2;
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            data->size = 4;
            data->alignment = 4;
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_WORD:
        case KEFIR_IR_TYPE_INT64:
            data->size = 8;
            data->alignment = 8;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-integral type");
    }
    data->aligned = true;
    return update_compound_type_layout(compound_type_layout, data, typeentry);
}

static kefir_result_t calculate_sse_layout(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               const struct kefir_ir_typeentry *typeentry,
                               void *payload) {
    UNUSED(type);
    struct compound_type_layout *compound_type_layout = (struct compound_type_layout *) payload;
    struct kefir_amd64_sysv_data_layout *data =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(compound_type_layout->vector, index);
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_FLOAT32:
            data->size = 4;
            data->alignment = 4;
            break;

        case KEFIR_IR_TYPE_FLOAT64:
            data->size = 8;
            data->alignment = 8;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-floating point type");
    }
    data->aligned = true;
    return update_compound_type_layout(compound_type_layout, data, typeentry);
}

static kefir_result_t calculate_amorphous_layout(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               const struct kefir_ir_typeentry *typeentry,
                               void *payload) {
    UNUSED(type);
    struct compound_type_layout *compound_type_layout = (struct compound_type_layout *) payload;
    struct kefir_amd64_sysv_data_layout *data =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(compound_type_layout->vector, index);
    
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_PAD:
            data->size = typeentry->param;
            data->alignment = 1;
            break;

        case KEFIR_IR_TYPE_MEMORY:
            data->size = typeentry->param;
            data->alignment = 1;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered structured type");
    }
    data->aligned = true;
    return update_compound_type_layout(compound_type_layout, data, typeentry);
}

static kefir_result_t calculate_struct_union_layout(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               const struct kefir_ir_typeentry *typeentry,
                               void *payload) {
    struct compound_type_layout *compound_type_layout = (struct compound_type_layout *) payload;
    struct kefir_amd64_sysv_data_layout *data =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(compound_type_layout->vector, index);
    struct compound_type_layout nested_compound_type_layout = {
        .visitor = compound_type_layout->visitor,
        .vector = compound_type_layout->vector,
        .offset = 0,
        .max_alignment = 0,
        .max_size = 0,
        .aggregate = typeentry->typecode == KEFIR_IR_TYPE_STRUCT,
        .aligned = true
    };
    REQUIRE_OK(kefir_ir_type_visitor_traverse_subtrees(type,
        compound_type_layout->visitor, (void *) &nested_compound_type_layout, index + 1, typeentry->param));
    data->alignment = nested_compound_type_layout.max_alignment;
    data->aligned = nested_compound_type_layout.aligned;
    data->size = pad_size(
        typeentry->typecode == KEFIR_IR_TYPE_STRUCT
            ? nested_compound_type_layout.offset
            :  nested_compound_type_layout.max_size,
        data->alignment
    );
    return update_compound_type_layout(compound_type_layout, data, typeentry);
}

static kefir_result_t calculate_array_layout(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               const struct kefir_ir_typeentry *typeentry,
                               void *payload) {
    struct compound_type_layout *compound_type_layout = (struct compound_type_layout *) payload;
    struct kefir_amd64_sysv_data_layout *data =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(compound_type_layout->vector, index);
    struct compound_type_layout nested_compound_type_layout = {
        .visitor = compound_type_layout->visitor,
        .vector = compound_type_layout->vector,
        .offset = 0,
        .max_alignment = 0,
        .max_size = 0,
        .aggregate = false,
        .aligned = true
    };
    REQUIRE_OK(kefir_ir_type_visitor_traverse_subtrees(type,
        compound_type_layout->visitor, (void *) &nested_compound_type_layout, index + 1, 1));
    data->alignment = nested_compound_type_layout.max_alignment;
    data->aligned = nested_compound_type_layout.aligned;
    data->size = nested_compound_type_layout.max_size * typeentry->param;
    return update_compound_type_layout(compound_type_layout, data, typeentry);
}

static kefir_result_t calculate_layout(const struct kefir_ir_type *type,
                                   struct kefir_vector *vector) {
    const kefir_size_t length = kefir_ir_type_length(type);
    struct kefir_ir_type_visitor visitor;
    struct compound_type_layout compound_type_layout = {
        .visitor = &visitor,
        .vector = vector,
        .offset = 0,
        .max_alignment = 0,
        .max_size = 0,
        .aggregate = true,
        .aligned = true
    };
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    visitor.visit[KEFIR_IR_TYPE_INT8] = calculate_integer_layout;
    visitor.visit[KEFIR_IR_TYPE_INT16] = calculate_integer_layout;
    visitor.visit[KEFIR_IR_TYPE_INT32] = calculate_integer_layout;
    visitor.visit[KEFIR_IR_TYPE_INT64] = calculate_integer_layout;
    visitor.visit[KEFIR_IR_TYPE_CHAR] = calculate_integer_layout;
    visitor.visit[KEFIR_IR_TYPE_SHORT] = calculate_integer_layout;
    visitor.visit[KEFIR_IR_TYPE_INT] = calculate_integer_layout;
    visitor.visit[KEFIR_IR_TYPE_LONG] = calculate_integer_layout;
    visitor.visit[KEFIR_IR_TYPE_WORD] = calculate_integer_layout;
    visitor.visit[KEFIR_IR_TYPE_FLOAT32] = calculate_sse_layout;
    visitor.visit[KEFIR_IR_TYPE_FLOAT64] = calculate_sse_layout;
    visitor.visit[KEFIR_IR_TYPE_PAD] = calculate_amorphous_layout;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = calculate_amorphous_layout;
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = calculate_struct_union_layout;
    visitor.visit[KEFIR_IR_TYPE_UNION] = calculate_struct_union_layout;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = calculate_array_layout;
    return kefir_ir_type_visitor_traverse_subtrees(type, &visitor, (void *) &compound_type_layout, 0, length);
}

kefir_result_t kefir_amd64_sysv_type_layout(const struct kefir_ir_type *type,
                                          struct kefir_mem *mem,
                                          struct kefir_vector *vector) {
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(vector != NULL, KEFIR_MALFORMED_ARG);
    const kefir_size_t length = kefir_ir_type_length(type);
    REQUIRE_OK(kefir_vector_alloc(mem,
                                sizeof(struct kefir_amd64_sysv_data_layout),
                                length,
                                vector));
    kefir_result_t res = kefir_vector_extend(vector, length);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, vector);
        return res;
    });
    res = calculate_layout(type, vector);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, vector);
        return res;
    });
    return KEFIR_OK;
}

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
    data->registers[0] = info->integer_register++;
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
    data->registers[0] = info->sse_register++;
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
    visitor.visit[KEFIR_IR_TYPE_INT8] = allocate_integer;
    visitor.visit[KEFIR_IR_TYPE_INT16] = allocate_integer;
    visitor.visit[KEFIR_IR_TYPE_INT32] = allocate_integer;
    visitor.visit[KEFIR_IR_TYPE_INT64] = allocate_integer;
    visitor.visit[KEFIR_IR_TYPE_CHAR] = allocate_integer;
    visitor.visit[KEFIR_IR_TYPE_SHORT] = allocate_integer;
    visitor.visit[KEFIR_IR_TYPE_INT] = allocate_integer;
    visitor.visit[KEFIR_IR_TYPE_LONG] = allocate_integer;
    visitor.visit[KEFIR_IR_TYPE_WORD] = allocate_integer;
    visitor.visit[KEFIR_IR_TYPE_FLOAT32] = allocate_sse;
    visitor.visit[KEFIR_IR_TYPE_FLOAT64] = allocate_sse;
    return kefir_ir_type_visitor_traverse_subtrees(type, &visitor, (void *) &info, 0, length);
}

kefir_result_t kefir_amd64_sysv_type_param_allocation(const struct kefir_ir_type *type,
                                          struct kefir_mem *mem,
                                          const struct kefir_vector *layout,
                                          struct kefir_vector *allocation) {
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(layout != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(allocation != NULL, KEFIR_MALFORMED_ARG);
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