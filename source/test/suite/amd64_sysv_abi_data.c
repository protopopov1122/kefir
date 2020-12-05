#include "kefir/test/unit_test.h"
#include "kefir/ir/type.h"
#include "kefir/codegen/amd64/system-v/abi_data.h"

#define ASSERT_DATA_ALLOC(vector, index, _size, _alignment, offset) \
    do { \
        struct kefir_amd64_sysv_data_allocation *param = \
                (struct kefir_amd64_sysv_data_allocation *) kefir_vector_at((vector), (index)); \
        ASSERT(param->layout.size == (_size)); \
        ASSERT(param->layout.alignment == (_alignment)); \
        ASSERT(param->layout.relative_offset == (offset)); \
    } while (0)

DEFINE_CASE(amd64_sysv_abi_data_test1, "AMD64 System V ABI - multiple scalars")
    struct kefir_ir_type type;
    struct kefir_vector vector;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 3, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_data_allocate(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 8, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 1, 1, 1, 8);
    ASSERT_DATA_ALLOC(&vector, 2, 4, 4, 12);
    ASSERT_OK(kefir_vector_free(&kft_mem, &vector));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test2, "AMD64 System V ABI - single struct")
    struct kefir_ir_type type;
    struct kefir_vector vector;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 5, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 4));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_MEMORY, 0, 13));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_data_allocate(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 28, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 1, 8, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 2, 1, 1, 8);
    ASSERT_DATA_ALLOC(&vector, 3, 13, 1, 9);
    ASSERT_DATA_ALLOC(&vector, 4, 4, 4, 24);
    ASSERT_OK(kefir_vector_free(&kft_mem, &vector));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test3, "AMD64 System V ABI - single union")
    struct kefir_ir_type type;
    struct kefir_vector vector;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 5, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_UNION, 0, 4));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_MEMORY, 0, 13));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_data_allocate(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 13, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 1, 8, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 2, 1, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 3, 13, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 4, 4, 4, 0);
    ASSERT_OK(kefir_vector_free(&kft_mem, &vector));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test4, "AMD64 System V ABI - nested structs")
    struct kefir_ir_type type;
    struct kefir_vector vector;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 10, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 3));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 4));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_WORD, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_UNION, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_MEMORY, 0, 20));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_CHAR, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_data_allocate(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 56, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 1, 1, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 2, 37, 8, 8);
    ASSERT_DATA_ALLOC(&vector, 3, 8, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 4, 8, 8, 8);
    ASSERT_DATA_ALLOC(&vector, 5, 20, 4, 16);
    ASSERT_DATA_ALLOC(&vector, 6, 4, 4, 0);
    ASSERT_DATA_ALLOC(&vector, 7, 20, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 8, 1, 1, 36);
    ASSERT_DATA_ALLOC(&vector, 9, 8, 8, 48);
    ASSERT_OK(kefir_vector_free(&kft_mem, &vector));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE