#include <stdalign.h>
#include <inttypes.h>
#include "kefir/test/unit_test.h"
#include "kefir/ir/type.h"
#include "kefir/ir/builder.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"

#define ASSERT_PARAM_REGISTER_ALLOCATION(allocation, index, _klass, _integer, _sse) \
    do { \
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc, \
            kefir_vector_at((allocation), (index))); \
        ASSERT(alloc->klass == (_klass)); \
        ASSERT(alloc->location.integer_register == (_integer)); \
        ASSERT(alloc->location.sse_register == (_sse)); \
    } while (0)

#define ASSERT_PARAM_STACK_ALLOCATION(allocation, index, _offset) \
    do { \
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc, \
            kefir_vector_at((allocation), (index))); \
        ASSERT(alloc->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY); \
        ASSERT(alloc->location.stack_offset == (_offset)); \
    } while (0)

#define ASSERT_NESTED_ALLOCATION(allocation, index, _klass, _location, _offset) \
    do { \
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc, \
            kefir_vector_at((allocation), (index))); \
        ASSERT(alloc->container_reference.qword->klass == (_klass)); \
        ASSERT(alloc->container_reference.qword->location == (_location)); \
        ASSERT(alloc->container_reference.offset == (_offset)); \
    } while (0)

#define NONE KEFIR_AMD64_SYSV_PARAMETER_LOCATION_NONE
DEFINE_CASE(amd64_sysv_abi_allocation_test1, "AMD64 System V ABI - parameter allocation")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 9, &type));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    struct kefir_amd64_sysv_parameter_location location = {0};
    ASSERT_OK(kefir_amd64_sysv_parameter_allocate(&kft_mem, &type, &layout, &allocation, &location));
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 0, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 3, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_INTEGER, 4, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_INTEGER, 5, NONE);
    ASSERT_PARAM_STACK_ALLOCATION(&allocation, 6, 0);
    ASSERT_PARAM_STACK_ALLOCATION(&allocation, 7, 8);
    ASSERT_PARAM_STACK_ALLOCATION(&allocation, 8, 16);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_allocation_test2, "AMD64 System V ABI - parameter allocation #2")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 15, &type));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_PAD, 0, 10));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_MEMORY, 0, 10));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT16, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    struct kefir_amd64_sysv_parameter_location location = {0};
    ASSERT_OK(kefir_amd64_sysv_parameter_allocate(&kft_mem, &type, &layout, &allocation, &location));
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 0, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_SSE, NONE, 0);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_SSE, NONE, 1);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_SSE, NONE, 2);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 6, KEFIR_AMD64_SYSV_PARAM_INTEGER, 3, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 7, KEFIR_AMD64_SYSV_PARAM_INTEGER, 4, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 8, KEFIR_AMD64_SYSV_PARAM_INTEGER, 5, NONE);
    ASSERT_PARAM_STACK_ALLOCATION(&allocation, 9, 0);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 10, KEFIR_AMD64_SYSV_PARAM_SSE, NONE, 3);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 11, KEFIR_AMD64_SYSV_PARAM_NO_CLASS, NONE, NONE);
    ASSERT_PARAM_STACK_ALLOCATION(&allocation, 12, 8);
    ASSERT_PARAM_STACK_ALLOCATION(&allocation, 13, 24);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 14, KEFIR_AMD64_SYSV_PARAM_SSE, NONE, 4);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_allocation_test3, "AMD64 System V ABI - parameter allocation #3")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 6, &type));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_STRUCT, 0, 4));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_ARRAY, 0, 8));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_CHAR, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    struct kefir_amd64_sysv_parameter_location location = {0};
    ASSERT_OK(kefir_amd64_sysv_parameter_allocate(&kft_mem, &type, &layout, &allocation, &location));
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 0, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_NO_CLASS, 1, NONE);
    ASSERT_NESTED_ALLOCATION(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 0);
    ASSERT_NESTED_ALLOCATION(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_INTEGER, 1, 4);
    ASSERT_NESTED_ALLOCATION(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, 0);
    ASSERT_NESTED_ALLOCATION(&allocation, 6, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, 1);
    ASSERT_NESTED_ALLOCATION(&allocation, 7, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, 2);
    ASSERT_NESTED_ALLOCATION(&allocation, 8, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, 3);
    ASSERT_NESTED_ALLOCATION(&allocation, 9, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, 4);
    ASSERT_NESTED_ALLOCATION(&allocation, 10, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, 5);
    ASSERT_NESTED_ALLOCATION(&allocation, 11, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, 6);
    ASSERT_NESTED_ALLOCATION(&allocation, 12, KEFIR_AMD64_SYSV_PARAM_INTEGER, 2, 7);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_allocation_test4, "AMD64 System V ABI - parameter allocation #4")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 4, &type));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT8, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_STRUCT, 0, 1));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_ARRAY, 0, 4));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    struct kefir_amd64_sysv_parameter_location location = {0};
    ASSERT_OK(kefir_amd64_sysv_parameter_allocate(&kft_mem, &type, &layout, &allocation, &location));
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 0, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, NONE);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_NO_CLASS, NONE, 0);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 2, KEFIR_AMD64_SYSV_PARAM_NO_CLASS, NONE, NONE);
    ASSERT_NESTED_ALLOCATION(&allocation, 3, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 0);
    ASSERT_NESTED_ALLOCATION(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_SSE, 0, 4);
    ASSERT_NESTED_ALLOCATION(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_SSE, 1, 0);
    ASSERT_NESTED_ALLOCATION(&allocation, 6, KEFIR_AMD64_SYSV_PARAM_SSE, 1, 4);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_allocation_test5, "AMD64 System V ABI - parameter allocation #5")
    struct kefir_ir_type type;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 6, &type));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_STRUCT, 0, 1));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT64, 1, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_STRUCT, 0, 1));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT64, 1, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_STRUCT, 0, 1));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type, &layout, &allocation));
    struct kefir_amd64_sysv_parameter_location location = {0};
    ASSERT_OK(kefir_amd64_sysv_parameter_allocate(&kft_mem, &type, &layout, &allocation, &location));
    ASSERT_PARAM_STACK_ALLOCATION(&allocation, 0, 0);
    ASSERT_PARAM_STACK_ALLOCATION(&allocation, 2, 8);
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 4, KEFIR_AMD64_SYSV_PARAM_NO_CLASS, 0, NONE);
    ASSERT_NESTED_ALLOCATION(&allocation, 5, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
END_CASE

DEFINE_CASE(amd64_sysv_abi_allocation_test6, "AMD64 System V ABI - parameter allocation #6")
    struct kefir_ir_type type1, type2;
    struct kefir_vector layout;
    struct kefir_vector allocation;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 8, &type1));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type1, KEFIR_IR_TYPE_ARRAY, 0, 10));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type1, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type1, KEFIR_IR_TYPE_STRUCT, 0, 1));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type1, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type1, KEFIR_IR_TYPE_STRUCT, 0, 1));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type1, KEFIR_IR_TYPE_INT64, 1, 0));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type1, KEFIR_IR_TYPE_STRUCT, 0, 1));
    ASSERT_OK(kefir_irbuilder_type_append_v(&kft_mem, &type1, KEFIR_IR_TYPE_INT64, 0, 0));
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 2, &type2));
    ASSERT_OK(kefir_irbuilder_type_append_e(&kft_mem, &type2, &type1, 2));
    ASSERT_OK(kefir_amd64_sysv_type_layout(&type2, &kft_mem, &layout));
    ASSERT_OK(kefir_amd64_sysv_parameter_classify(&kft_mem, &type2, &layout, &allocation));
    struct kefir_amd64_sysv_parameter_location location = {0};
    ASSERT_OK(kefir_amd64_sysv_parameter_allocate(&kft_mem, &type2, &layout, &allocation, &location));
    ASSERT_PARAM_REGISTER_ALLOCATION(&allocation, 0, KEFIR_AMD64_SYSV_PARAM_NO_CLASS, 0, NONE);
    ASSERT_NESTED_ALLOCATION(&allocation, 1, KEFIR_AMD64_SYSV_PARAM_INTEGER, 0, 0);
    ASSERT_OK(kefir_amd64_sysv_parameter_free(&kft_mem, &allocation));
    ASSERT_OK(kefir_vector_free(&kft_mem, &layout));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type2));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &type1));
END_CASE
#undef NONE
