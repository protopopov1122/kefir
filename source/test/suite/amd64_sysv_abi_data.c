#include <stdalign.h>
#include <inttypes.h>
#include "kefir/test/unit_test.h"
#include "kefir/ir/type.h"
#include "kefir/codegen/amd64/system-v/abi_data.h"
#include "kefir/codegen/amd64/system-v/abi_allocation.h"

#define ASSERT_DATA_ALLOC(vector, index, _size, _alignment, offset) \
    do { \
        struct kefir_amd64_sysv_data_layout *param = \
                (struct kefir_amd64_sysv_data_layout *) kefir_vector_at((vector), (index)); \
        ASSERT(param->size == (_size)); \
        ASSERT(param->alignment == (_alignment)); \
        ASSERT(param->relative_offset == (offset)); \
    } while (0)

#define ASSERT_PARAM_ALLOC(allocation, index, _type, _klass) \
    do { \
        struct kefir_amd64_sysv_input_parameter_allocation *alloc = \
            (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at((allocation), (index)); \
        ASSERT(alloc->type == (_type)); \
        ASSERT(alloc->klass == (_klass)); \
    } while (0)

#define ASSERT_NESTED_PARAM_ALLOC(allocation, _qword_index, _klass, _index, _offset) \
    do { \
        ASSERT_PARAM_ALLOC((allocation), (_qword_index), \
            KEFIR_AMD64_SYSV_INPUT_PARAM_NESTED, KEFIR_AMD64_SYSV_PARAM_NO_CLASS); \
        struct kefir_amd64_sysv_input_parameter_allocation *alloc = \
            (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at((allocation), (_qword_index)); \
        ASSERT(alloc->container_reference.qword->klass == (_klass)); \
        ASSERT(alloc->container_reference.qword->index == (_index)); \
        ASSERT(alloc->container_reference.offset == (_offset)); \
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

DEFINE_CASE(amd64_sysv_abi_data_test8, "AMD64 System V ABI - eightbyte allocation")
    struct kefir_ir_type type;
    struct kefir_vector vector;
    struct kefir_amd64_sysv_abi_qwords qwords;
    struct kefir_amd64_sysv_abi_qword_ref ref;
    kefir_size_t qword_count;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 11, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 10));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_MEMORY, 0, 10));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &vector));
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_count(&type, &vector, &qword_count));
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_alloc(&qwords, &kft_mem, qword_count));
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 1, &ref));
    ASSERT(ref.qword->index == 0 && ref.offset == 0);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 1, &ref));
    ASSERT(ref.qword->index == 0 && ref.offset == 1);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 4, 4, &ref));
    ASSERT(ref.qword->index == 0 && ref.offset == 4);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, 2, &ref));
    ASSERT(ref.qword->index == 1 && ref.offset == 0);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 8, 8, &ref));
    ASSERT(ref.qword->index == 2 && ref.offset == 0);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 4, 4, &ref));
    ASSERT(ref.qword->index == 3 && ref.offset == 0);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 1, &ref));
    ASSERT(ref.qword->index == 3 && ref.offset == 4);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 4, 4, &ref));
    ASSERT(ref.qword->index == 4 && ref.offset == 0);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 10, 1, &ref));
    ASSERT(ref.qword->index == 4 && ref.offset == 4);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_next(&qwords, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 1, &ref));
    ASSERT(ref.qword->index == 5 && ref.offset == 6);
    ASSERT_OK(kefir_amd64_sysv_abi_qwords_free(&qwords, &kft_mem));
    ASSERT_OK(kefir_vector_free(&kft_mem, &vector));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test9, "AMD64 System V ABI - parameter classification")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 5, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_input_parameter_allocate(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_INTEGER);
    ASSERT_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 4);
    ASSERT_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_SSE);
    ASSERT_OK(kefir_amd64_sysv_input_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test10, "AMD64 System V ABI - parameter classification #2")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 5, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_input_parameter_allocate(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_SSE);
    ASSERT_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_INTEGER);
    ASSERT_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 4);
    ASSERT_OK(kefir_amd64_sysv_input_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test11, "AMD64 System V ABI - parameter classification #3")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 6, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 3));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_input_parameter_allocate(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 4);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_SSE, 1, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_SSE, 1, 4);
    ASSERT_OK(kefir_amd64_sysv_input_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test12, "AMD64 System V ABI - parameter classification #4")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 10, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 9));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_input_parameter_allocate(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_MEMORY);
    ASSERT_OK(kefir_amd64_sysv_input_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test13, "AMD64 System V ABI - parameter classification #5")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 4, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 3));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 1, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_input_parameter_allocate(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_MEMORY);
    ASSERT_OK(kefir_amd64_sysv_input_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test14, "AMD64 System V ABI - parameter classification #6")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 11, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 5));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 4));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_input_parameter_allocate(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 2);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 4);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 0);
    ASSERT_PARAM_ALLOC(&allocation, 5, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 6, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 4);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 7, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 5);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 8, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 6);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 9, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 7);
    ASSERT_PARAM_ALLOC(&allocation, 10, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_INTEGER);
    ASSERT_OK(kefir_amd64_sysv_input_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test15, "AMD64 System V ABI - parameter classification #7")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 11, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 5));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 4));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_input_parameter_allocate(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_MEMORY);
    ASSERT_PARAM_ALLOC(&allocation, 10, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_INTEGER);
    ASSERT_OK(kefir_amd64_sysv_input_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_data_test16, "AMD64 System V ABI - parameter classification #8")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 4, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 4));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_input_parameter_allocate(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 1);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 2);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 3);
    ASSERT_PARAM_ALLOC(&allocation, 5, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 6, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 7, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 4);
    ASSERT_OK(kefir_amd64_sysv_input_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE