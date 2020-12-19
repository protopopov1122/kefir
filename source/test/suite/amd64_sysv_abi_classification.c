#include <stdalign.h>
#include <inttypes.h>
#include "kefir/test/unit_test.h"
#include "kefir/ir/type.h"
#include "kefir/codegen/amd64/system-v/abi_data.h"
#include "kefir/codegen/amd64/system-v/abi_allocation.h"

#define ASSERT_PARAM_ALLOC(allocation, index, _type, _klass) \
    do { \
        struct kefir_amd64_sysv_parameter_allocation *alloc = \
            (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at((allocation), (index)); \
        ASSERT(alloc->type == (_type)); \
        ASSERT(alloc->klass == (_klass)); \
    } while (0)

#define ASSERT_NESTED_PARAM_ALLOC(allocation, _qword_index, _klass, _index, _offset) \
    do { \
        ASSERT_PARAM_ALLOC((allocation), (_qword_index), \
            KEFIR_AMD64_SYSV_INPUT_PARAM_NESTED, KEFIR_AMD64_SYSV_PARAM_NO_CLASS); \
        struct kefir_amd64_sysv_parameter_allocation *alloc = \
            (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at((allocation), (_qword_index)); \
        ASSERT(alloc->container_reference.qword->klass == (_klass)); \
        ASSERT(alloc->container_reference.qword->index == (_index)); \
        ASSERT(alloc->container_reference.offset == (_offset)); \
    } while (0)

#define ASSERT_PARAM_REQUIREMENTS(allocation, index, _integer, _sse, _sseup, _memsize, _memalign) \
    do { \
        struct kefir_amd64_sysv_parameter_allocation *alloc = \
            (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at((allocation), (index)); \
        ASSERT(alloc->requirements.integer == (_integer)); \
        ASSERT(alloc->requirements.sse == (_sse)); \
        ASSERT(alloc->requirements.sseup == (_sseup)); \
        ASSERT(alloc->requirements.memory.size == (_memsize)); \
        ASSERT(alloc->requirements.memory.alignment == (_memalign)); \
    } while (0)

DEFINE_CASE(amd64_sysv_abi_classification_test1, "AMD64 System V ABI - parameter classification")
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
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_INTEGER);
    ASSERT_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 4);
    ASSERT_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_SSE);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test2, "AMD64 System V ABI - parameter classification #2")
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
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_SSE);
    ASSERT_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_INTEGER);
    ASSERT_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 4);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test3, "AMD64 System V ABI - parameter classification #3")
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
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 4);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_SSE, 1, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_SSE, 1, 4);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test4, "AMD64 System V ABI - parameter classification #4")
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
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_MEMORY);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test5, "AMD64 System V ABI - parameter classification #5")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 4, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 3));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 1, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_MEMORY);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test6, "AMD64 System V ABI - parameter classification #6")
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
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
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
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test7, "AMD64 System V ABI - parameter classification #7")
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
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_MEMORY);
    ASSERT_PARAM_ALLOC(&allocation, 10, KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE, KEFIR_AMD64_SYSV_PARAM_INTEGER);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test8, "AMD64 System V ABI - parameter classification #8")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 4, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 4));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 1);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 2);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 3);
    ASSERT_PARAM_ALLOC(&allocation, 5, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 6, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 7, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 4);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test9, "AMD64 System V ABI - parameter classification #9")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 9, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 1));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 3));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_CHAR, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 1);
    ASSERT_PARAM_ALLOC(&allocation, 6, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 7, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 2);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 8, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 4);
    ASSERT_PARAM_ALLOC(&allocation, 9, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 10, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 6);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 11, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 7);
    ASSERT_PARAM_ALLOC(&allocation, 12, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_PARAM_ALLOC(&allocation, 13, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 14, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 15, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 1);
    ASSERT_PARAM_ALLOC(&allocation, 16, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 17, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 2);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 18, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 4);
    ASSERT_PARAM_ALLOC(&allocation, 19, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 20, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 6);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 21, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 7);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test10, "AMD64 System V ABI - parameter classification #10")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 5, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 4);
    ASSERT_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 6, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 4);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test11, "AMD64 System V ABI - parameter classification #11")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 5, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 3));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_MEMORY);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test12, "AMD64 System V ABI - parameter classification #12")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 5, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_UNION, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 3));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 8));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_CHAR, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 4);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_SSE, 1, 0);
    ASSERT_PARAM_ALLOC(&allocation, 5, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 6, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 7, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 1);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 8, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 2);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 9, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 3);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 10, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 4);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 11, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 5);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 12, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 6);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 13, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 7);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test13, "AMD64 System V ABI - parameter classification #13")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 5, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_UNION, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 8));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_CHAR, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_ALLOC(&allocation, 0, KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_PARAM_ALLOC(&allocation, 1, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_PARAM_ALLOC(&allocation, 3, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 1);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 6, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 2);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 7, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 3);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 8, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 4);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 9, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 5);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 10, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 6);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 11, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 7);
    ASSERT_PARAM_ALLOC(&allocation, 12, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 13, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 0);
    ASSERT_PARAM_ALLOC(&allocation, 14, KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER, KEFIR_AMD64_SYSV_PARAM_NO_CLASS);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 15, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 0);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 16, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 1);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 17, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 2);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 18, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 3);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 19, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 4);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 20, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 5);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 21, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 6);
    ASSERT_NESTED_PARAM_ALLOC(&allocation, 22, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 7);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_classification_test14, "AMD64 System V ABI - parameter statistics")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 9, &type));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_ARRAY, 0, 10));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_CHAR, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_UNION, 0, 2));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_BOOL, 0, 0));
    ASSERT_OK(kefir_ir_type_append_v(&type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    ASSERT_PARAM_REQUIREMENTS(&allocation, 0, 1, 0, 0, 0, 0);
    ASSERT_PARAM_REQUIREMENTS(&allocation, 1, 0, 0, 0, 24, 8);
    ASSERT_PARAM_REQUIREMENTS(&allocation, 14, 0, 1, 0, 0, 0);
    ASSERT_PARAM_REQUIREMENTS(&allocation, 15, 1, 0, 0, 0, 0);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE
