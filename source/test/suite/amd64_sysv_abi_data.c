#include <stdalign.h>
#include <inttypes.h>
#include "kefir/test/unit_test.h"
#include "kefir/ir/type.h"
#include "kefir/codegen/amd64/system-v/abi_data.h"

#define ASSERT_DATA_ALLOC(vector, index, _size, _alignment, offset) \
    do { \
        struct kefir_amd64_sysv_data_layout *param = \
                (struct kefir_amd64_sysv_data_layout *) kefir_vector_at((vector), (index)); \
        ASSERT(param->size == (_size)); \
        ASSERT(param->alignment == (_alignment)); \
        ASSERT(param->relative_offset == (offset)); \
    } while (0)

DEFINE_CASE(amd64_sysv_abi_data_test1, "AMD64 System V ABI - multiple scalars")
    struct kefir_ir_type type;
    struct kefir_vector vector;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 3, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &vector));
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
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 32, 8, 0);
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
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 16, 8, 0);
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
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 11, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 4));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 4));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_WORD, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_UNION, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_MEMORY, 0, 20));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_CHAR, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 64, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 1, 1, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 2, 40, 8, 8);
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

DEFINE_CASE(amd64_sysv_abi_data_test5, "AMD64 System V ABI - unaligned data")
    struct kefir_ir_type type;
    struct kefir_vector vector;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 6, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 3));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 1, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 1, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_WORD, 1, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT64, 1, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 25, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 1, 1, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 2, 16, 1, 1);
    ASSERT_DATA_ALLOC(&vector, 3, 8, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 4, 8, 1, 8);
    ASSERT_DATA_ALLOC(&vector, 5, 8, 1, 17);
    ASSERT_OK(kefir_vector_free(&kft_mem, &vector));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test6, "AMD64 System V ABI - array")
    struct kefir_ir_type type;
    struct kefir_vector vector;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 4, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 10));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_WORD, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 160, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 1, 16, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 2, 1, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 3, 8, 8, 8);
    ASSERT_OK(kefir_vector_free(&kft_mem, &vector));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test7, "AMD64 System V ABI - array #2")
    struct kefir_ir_type type;
    struct kefir_vector vector;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 8, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 5));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 100));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_CHAR, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 3));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &vector));
    ASSERT_DATA_ALLOC(&vector, 0, 600, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 1, 120, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 2, 100, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 3, 1, 1, 0);
    ASSERT_DATA_ALLOC(&vector, 4, 16, 8, 104);
    ASSERT_DATA_ALLOC(&vector, 5, 8, 8, 0);
    ASSERT_DATA_ALLOC(&vector, 6, 2, 2, 8);
    ASSERT_DATA_ALLOC(&vector, 7, 2, 2, 10);
    ASSERT_OK(kefir_vector_free(&kft_mem, &vector));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE