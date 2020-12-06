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
                                            kefir_ir_typecode_t typecode,
                                            kefir_int64_t param,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typecode);
    UNUSED(param);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Encountered not supported type code while traversing type");
}

struct layout_info {
    struct kefir_ir_type_visitor *visitor;
    struct kefir_vector *vector;
    kefir_size_t offset;
    kefir_size_t max_alignment;
    kefir_size_t max_size;
    bool aggregate;
};

static kefir_result_t update_layout_info(struct layout_info *layout_info, struct kefir_amd64_sysv_data_allocation *data) {
    const kefir_size_t modulo = layout_info->offset % data->layout.alignment;
    if (modulo != 0 && layout_info->aggregate) {
        layout_info->offset += data->layout.alignment - modulo;
    }
    layout_info->max_alignment = MAX(layout_info->max_alignment, data->layout.alignment);
    layout_info->max_size = MAX(layout_info->max_size, data->layout.size);
    data->layout.relative_offset = layout_info->offset;
    if (layout_info->aggregate) {
        layout_info->offset += data->layout.size;
    }
    return KEFIR_OK;
}

static kefir_result_t assign_class_integer(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               kefir_ir_typecode_t typecode,
                               kefir_int64_t param,
                               void *payload) {
    UNUSED(type);
    UNUSED(param);
    struct layout_info *layout_info = (struct layout_info *) payload;
    struct kefir_amd64_sysv_data_allocation *palloc =
        (struct kefir_amd64_sysv_data_allocation *) kefir_vector_at(layout_info->vector, index);
    palloc->dataclass = KEFIR_AMD64_SYSV_PARAM_INTEGER;
    palloc->location.offset = 0;
    switch (typecode) {
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_INT8:
            palloc->layout.size = 1;
            palloc->layout.alignment = 1;
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            palloc->layout.size = 2;
            palloc->layout.alignment = 2;
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            palloc->layout.size = 4;
            palloc->layout.alignment = 4;
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_WORD:
        case KEFIR_IR_TYPE_INT64:
            palloc->layout.size = 8;
            palloc->layout.alignment = 8;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-integral type");
    }
    return update_layout_info(layout_info, palloc);
}

static kefir_result_t assign_class_sse(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               kefir_ir_typecode_t typecode,
                               kefir_int64_t param,
                               void *payload) {
    UNUSED(type);
    UNUSED(param);
    struct layout_info *layout_info = (struct layout_info *) payload;
    struct kefir_amd64_sysv_data_allocation *palloc =
        (struct kefir_amd64_sysv_data_allocation *) kefir_vector_at(layout_info->vector, index);
    palloc->dataclass = KEFIR_AMD64_SYSV_PARAM_SSE;
    palloc->location.offset = 0;
    switch (typecode) {
        case KEFIR_IR_TYPE_FLOAT32:
            palloc->layout.size = 4;
            palloc->layout.alignment = 4;
            break;

        case KEFIR_IR_TYPE_FLOAT64:
            palloc->layout.size = 8;
            palloc->layout.alignment = 8;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-floating point type");
    }
    return update_layout_info(layout_info, palloc);
}

static kefir_result_t assign_class_amorphous(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               kefir_ir_typecode_t typecode,
                               kefir_int64_t param,
                               void *payload) {
    UNUSED(type);
    struct layout_info *layout_info = (struct layout_info *) payload;
    struct kefir_amd64_sysv_data_allocation *palloc =
        (struct kefir_amd64_sysv_data_allocation *) kefir_vector_at(layout_info->vector, index);
    
    palloc->location.offset = 0;
    switch (typecode) {
        case KEFIR_IR_TYPE_PAD:
            palloc->dataclass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
            palloc->layout.size = param;
            palloc->layout.alignment = 1;
            break;

        case KEFIR_IR_TYPE_MEMORY:
            palloc->dataclass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
            palloc->layout.size = param;
            palloc->layout.alignment = 1;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered structured type");
    }
    return update_layout_info(layout_info, palloc);
}

static kefir_result_t assign_class_struct_union(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               kefir_ir_typecode_t typecode,
                               kefir_int64_t param,
                               void *payload) {
    struct layout_info *layout_info = (struct layout_info *) payload;
    struct kefir_amd64_sysv_data_allocation *palloc =
        (struct kefir_amd64_sysv_data_allocation *) kefir_vector_at(layout_info->vector, index);
    palloc->location.offset = 0;
    struct layout_info nested_layout_info = {
        .visitor = layout_info->visitor,
        .vector = layout_info->vector,
        .offset = 0,
        .max_alignment = 0,
        .max_size = 0,
        .aggregate = typecode == KEFIR_IR_TYPE_STRUCT
    };
    REQUIRE_OK(kefir_ir_type_visitor_traverse(type,
        layout_info->visitor, (void *) &nested_layout_info, index + 1, param));
    palloc->dataclass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS; // TODO
    palloc->location.offset = 0;
    palloc->layout.size = typecode == KEFIR_IR_TYPE_STRUCT
        ? nested_layout_info.offset
        :  nested_layout_info.max_size;
    palloc->layout.alignment = nested_layout_info.max_alignment;
    return update_layout_info(layout_info, palloc);
}

static kefir_result_t calculate_layout(const struct kefir_ir_type *type,
                                   struct kefir_vector *vector) {
    const kefir_size_t length = kefir_ir_type_length(type);
    struct kefir_ir_type_visitor visitor;
    struct layout_info layout_info = {
        .visitor = &visitor,
        .vector = vector,
        .offset = 0,
        .max_alignment = 0,
        .max_size = 0,
        .aggregate = true
    };
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    visitor.visit[KEFIR_IR_TYPE_INT8] = assign_class_integer;
    visitor.visit[KEFIR_IR_TYPE_INT16] = assign_class_integer;
    visitor.visit[KEFIR_IR_TYPE_INT32] = assign_class_integer;
    visitor.visit[KEFIR_IR_TYPE_INT64] = assign_class_integer;
    visitor.visit[KEFIR_IR_TYPE_CHAR] = assign_class_integer;
    visitor.visit[KEFIR_IR_TYPE_SHORT] = assign_class_integer;
    visitor.visit[KEFIR_IR_TYPE_INT] = assign_class_integer;
    visitor.visit[KEFIR_IR_TYPE_LONG] = assign_class_integer;
    visitor.visit[KEFIR_IR_TYPE_WORD] = assign_class_integer;
    visitor.visit[KEFIR_IR_TYPE_FLOAT32] = assign_class_sse;
    visitor.visit[KEFIR_IR_TYPE_FLOAT64] = assign_class_sse;
    visitor.visit[KEFIR_IR_TYPE_PAD] = assign_class_amorphous;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = assign_class_amorphous;
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = assign_class_struct_union;
    visitor.visit[KEFIR_IR_TYPE_UNION] = assign_class_struct_union;
    return kefir_ir_type_visitor_traverse(type, &visitor, (void *) &layout_info, 0, length);
}

struct parameter_allocation_info {
    struct kefir_vector *vector;

    kefir_size_t integer_register;
    kefir_size_t sse_register;
    kefir_size_t sseup_register;
    kefir_size_t memory_offset;
};

static kefir_result_t allocate_integer(const struct kefir_ir_type *type,
                                     kefir_size_t index,
                                     kefir_ir_typecode_t typecode,
                                     kefir_int64_t param,
                                     void *payload) {
    UNUSED(type);
    UNUSED(typecode);
    UNUSED(param);
    struct parameter_allocation_info *info = (struct parameter_allocation_info *) payload;
    struct kefir_amd64_sysv_data_allocation *palloc =
        (struct kefir_amd64_sysv_data_allocation *) kefir_vector_at(info->vector, index);
    if (info->integer_register == KEFIR_AMD64_SYSV_INTEGER_REGISTER_COUNT) {
        return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
            KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Available number of integral registers exceeded");
    }
    palloc->location.registers[0] = info->integer_register++;
    return KEFIR_OK;
}

static kefir_result_t allocate_sse(const struct kefir_ir_type *type,
                                 kefir_size_t index,
                                 kefir_ir_typecode_t typecode,
                                 kefir_int64_t param,
                                 void *payload) {
    UNUSED(type);
    UNUSED(typecode);
    UNUSED(param);
    struct parameter_allocation_info *info = (struct parameter_allocation_info *) payload;
    struct kefir_amd64_sysv_data_allocation *palloc =
        (struct kefir_amd64_sysv_data_allocation *) kefir_vector_at(info->vector, index);
    if (info->sse_register == KEFIR_AMD64_SYSV_SSE_REGISTER_COUNT) {
        return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
            KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Available number of SSE register exceeded");
    }
    palloc->location.registers[0] = info->sse_register++;
    return KEFIR_OK;
}

static kefir_result_t allocate_data(const struct kefir_ir_type *type,
                                        struct kefir_vector *vector) {
    const kefir_size_t length = kefir_ir_type_length(type);
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    struct parameter_allocation_info info = {
        .vector = vector,
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
    return kefir_ir_type_visitor_traverse(type, &visitor, (void *) &info, 0, length);
}

kefir_result_t kefir_amd64_sysv_data_allocate(const struct kefir_ir_type *type,
                                          struct kefir_mem *mem,
                                          struct kefir_vector *vector) {
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(vector != NULL, KEFIR_MALFORMED_ARG);
    const kefir_size_t length = kefir_ir_type_length(type);
    REQUIRE_OK(kefir_vector_alloc(mem,
                                sizeof(kefir_amd64_sysv_data_allocation_t),
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
    // res = allocate_data(type, vector);
    // REQUIRE_ELSE(res == KEFIR_OK, {
    //     kefir_vector_free(mem, vector);
    //     return res;
    // });
    return KEFIR_OK;
}