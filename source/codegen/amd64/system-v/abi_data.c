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

// struct class_assignment_info {
//     struct kefir_ir_type_visitor *visitor;
//     const struct kefir_vector *layout;
//     struct kefir_vector *allocation;

//     kefir_size_t nested_level;
//     kefir_amd64_sysv_data_class_t eightbyte_dataclass;
//     kefir_size_t eightbyte;
//     kefir_size_t eightbyte_index;
//     kefir_size_t eightbyte_offset;
// };

// static kefir_amd64_sysv_data_class_t derive_dataclass(kefir_amd64_sysv_data_class_t first,
//                                                     kefir_amd64_sysv_data_class_t second) {
//     if (first == second) {
//         return first;
//     }
//     if (first == KEFIR_AMD64_SYSV_PARAM_NO_CLASS) {
//         return second;
//     }
//     if  (second == KEFIR_AMD64_SYSV_PARAM_NO_CLASS) {
//         return first;
//     }
// #define ANY_OF(x, y, a) ((x) == (a) || (y) == (a))
//     if (ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_MEMORY)) {
//         return KEFIR_AMD64_SYSV_PARAM_MEMORY;
//     }
//     if (ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_INTEGER)) {
//         return KEFIR_AMD64_SYSV_PARAM_INTEGER;
//     }
//     if (ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_X87) ||
//         ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_X87UP) ||
//         ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_COMPLEX_X87)) {
//         return KEFIR_AMD64_SYSV_PARAM_MEMORY;
//     }
// #undef ANY_OF
//     return KEFIR_AMD64_SYSV_PARAM_SSE;
// }

// static void finalize_eightbyte(struct class_assignment_info *info,
//                                 struct kefir_vector *alloction_vec,
//                                 kefir_size_t end) {
//     for (kefir_size_t i = info->eightbyte_index; info->eightbyte_offset > 0 && i < end; i++) {
//         struct kefir_amd64_sysv_parameter_allocation *member =
//             (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(alloction_vec, i);
//         member->dataclass = info->eightbyte_dataclass;
//     }
// }

// static void allocate_eightbyte(struct class_assignment_info *info,
//                                 const struct kefir_amd64_sysv_data_layout *layout,
//                                 struct kefir_vector *alloction_vec,
//                                 kefir_size_t index) {
//     struct kefir_amd64_sysv_parameter_allocation *data =
//         (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(alloction_vec, index);
//     kefir_size_t offset = pad_size(info->eightbyte_offset, layout->alignment);
//     if (offset < 8) {
//         info->eightbyte_offset = offset + layout->size;
//         data->eightbyte = info->eightbyte;
//         data->eightbyte_offset = offset;
//     } else {
//         finalize_eightbyte(info, alloction_vec, index);
//         info->eightbyte++;
//         info->eightbyte_index = index;
//         info->eightbyte_offset = 0;
//         info->eightbyte_dataclass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
//         data->eightbyte = info->eightbyte;
//         data->eightbyte_offset = offset;
//     }
// }

// static void assign_type_class(struct class_assignment_info *info,
//                               const struct kefir_amd64_sysv_data_layout *layout,
//                               struct kefir_vector *alloction_vec,
//                               kefir_size_t index,
//                               kefir_amd64_sysv_data_class_t dataclass) {
//     if (info->nested_level == 0) {
//         finalize_eightbyte(info, alloction_vec, index);
//         info->eightbyte++;
//         info->eightbyte_index = index;
//         info->eightbyte_offset = 0;
//         info->eightbyte_dataclass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
//         data->dataclass = dataclass;
//         data->eightbyte = info->eightbyte;
//         data->eightbyte_offset = 0;
//     } else {
//         allocate_eightbyte(info, layout, alloction_vec, index);
//         info->eightbyte_dataclass = derive_dataclass(info->eightbyte_dataclass, dataclass);
//     }
// }

// static kefir_result_t assign_integer_class(const struct kefir_ir_type *type,
//                                  kefir_size_t index,
//                                  const struct kefir_ir_typeentry *typeentry,
//                                  void *payload) {
//     UNUSED(type);
//     UNUSED(typeentry);
//     struct class_assignment_info *info = (struct class_assignment_info *) payload;
//     const struct kefir_amd64_sysv_data_layout *layout =
//         (const struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
//     struct kefir_amd64_sysv_parameter_allocation *data =
//         (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(info->allocation, index);
//     assign_type_class(info, layout, info->allocation, index, KEFIR_AMD64_SYSV_PARAM_INTEGER);
//     return KEFIR_OK;
// }

// static kefir_result_t assign_sse_class(const struct kefir_ir_type *type,
//                                  kefir_size_t index,
//                                  const struct kefir_ir_typeentry *typeentry,
//                                  void *payload) {
//     UNUSED(type);
//     UNUSED(typeentry);
//     struct class_assignment_info *info = (struct class_assignment_info *) payload;
//     const struct kefir_amd64_sysv_data_layout *layout =
//         (const struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
//     struct kefir_amd64_sysv_parameter_allocation *data =
//         (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(info->allocation, index);
//     assign_type_class(info, layout, info->allocation, index, KEFIR_AMD64_SYSV_PARAM_SSE);
//     return KEFIR_OK;
// }

// static kefir_result_t assign_pad_class(const struct kefir_ir_type *type,
//                                  kefir_size_t index,
//                                  const struct kefir_ir_typeentry *typeentry,
//                                  void *payload) {
//     UNUSED(type);
//     UNUSED(typeentry);
//     struct class_assignment_info *info = (struct class_assignment_info *) payload;
//     const struct kefir_amd64_sysv_data_layout *layout =
//         (const struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
//     struct kefir_amd64_sysv_parameter_allocation *data =
//         (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(info->allocation, index);
//     assign_type_class(info, layout, info->allocation, index, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
//     return KEFIR_OK;
// }

// static kefir_result_t assign_memory_class(const struct kefir_ir_type *type,
//                                  kefir_size_t index,
//                                  const struct kefir_ir_typeentry *typeentry,
//                                  void *payload) {
//     UNUSED(type);
//     UNUSED(typeentry);
//     struct class_assignment_info *info = (struct class_assignment_info *) payload;
//     const struct kefir_amd64_sysv_data_layout *layout =
//         (const struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
//     struct kefir_amd64_sysv_parameter_allocation *data =
//         (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(info->allocation, index);
//     assign_type_class(info, layout, info->allocation, index, KEFIR_AMD64_SYSV_PARAM_MEMORY);
//     return KEFIR_OK;
// }

// static kefir_result_t assign_struct_class(const struct kefir_ir_type *type,
//                                  kefir_size_t index,
//                                  const struct kefir_ir_typeentry *typeentry,
//                                  void *payload) {
//     struct class_assignment_info *info = (struct class_assignment_info *) payload;
//     struct kefir_amd64_sysv_data_layout *layout =
//         (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
//     struct kefir_amd64_sysv_parameter_allocation *data =
//         (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(info->allocation, index);
//     if (layout->size > 8 * 8 || !layout->aligned) {
//         data->dataclass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
//         return KEFIR_OK;
//     }
//     info->nested_level++;
//     REQUIRE_OK(kefir_ir_type_visitor_traverse_subtrees(type, info->visitor, (void *) info, index, typeentry->param));
//     info->nested_level--;
//     return KEFIR_OK;
// }

// static kefir_result_t assign_param_classes(const struct kefir_ir_type *type,
//                                   const struct kefir_vector *layout,
//                                   struct kefir_vector *allocation) {
//     const kefir_size_t length = kefir_ir_type_length(type);
//     struct kefir_ir_type_visitor visitor;
//     REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
//     struct class_assignment_info info = {
//         .visitor = &visitor,
//         .layout = layout,
//         .allocation = allocation,
//         .nested_level = 0,
//         .eightbyte_dataclass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS,
//         .eightbyte = 0,
//         .eightbyte_offset
//     };
//     visitor.visit[KEFIR_IR_TYPE_INT8] = assign_integer_class;
//     visitor.visit[KEFIR_IR_TYPE_INT16] = assign_integer_class;
//     visitor.visit[KEFIR_IR_TYPE_INT32] = assign_integer_class;
//     visitor.visit[KEFIR_IR_TYPE_INT64] = assign_integer_class;
//     visitor.visit[KEFIR_IR_TYPE_CHAR] = assign_integer_class;
//     visitor.visit[KEFIR_IR_TYPE_SHORT] = assign_integer_class;
//     visitor.visit[KEFIR_IR_TYPE_INT] = assign_integer_class;
//     visitor.visit[KEFIR_IR_TYPE_LONG] = assign_integer_class;
//     visitor.visit[KEFIR_IR_TYPE_WORD] = assign_integer_class;
//     visitor.visit[KEFIR_IR_TYPE_FLOAT32] = assign_sse_class;
//     visitor.visit[KEFIR_IR_TYPE_FLOAT64] = assign_sse_class;
//     visitor.visit[KEFIR_IR_TYPE_PAD] = assign_pad_class;
//     visitor.visit[KEFIR_IR_TYPE_MEMORY] = assign_memory_class;
//     REQUIRE_OK(kefir_ir_type_visitor_traverse_subtrees(type, &visitor, (void *) &info, 0, length));
//     finalize_eightbyte(&info, allocation, length);
//     return KEFIR_OK;
// }

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