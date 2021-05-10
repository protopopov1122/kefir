#include "kefir/test/unit_test.h"
#include "kefir/codegen/amd64/system-v/platform.h"

DEFINE_CASE(amd64_sysv_abi_platform_bitfields1, "AMD64 System V ABI - platform bitfields #1")
    // struct kefir_ir_target_platform platform;
    // struct kefir_ir_bitfield_allocator bitfields;
    // ASSERT_OK(kefir_codegen_amd64_sysv_target_platform(&platform));

    // struct kefir_ir_type type;
    // struct kefir_irbuilder_type builder;
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));
    // ASSERT_OK(kefir_irbuilder_type_init(&kft_mem, &builder, &type));

    // ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_BITFIELD_ALLOCATOR(&kft_mem, &platform, &type, &bitfields));
    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_CHAR, 0, 0));
    // struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(&type, 0);

    // struct kefir_ir_bitfield bitfield;
    // ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 0, &bitfield));

    // do {
    //     struct kefir_ir_typeentry colocated = *typeentry;

    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 9, &bitfield) == KEFIR_OUT_OF_BOUNDS);
    //     ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 0, &bitfield));

    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 1, &bitfield));
    //     ASSERT(bitfield.offset == 0 && bitfield.width == 1);
    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 1, &bitfield));
    //     ASSERT(bitfield.offset == 1 && bitfield.width == 1);
    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 1, &bitfield));
    //     ASSERT(bitfield.offset == 2 && bitfield.width == 1);

    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 2, &bitfield));
    //     ASSERT(bitfield.offset == 3 && bitfield.width == 2);
    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 3, &bitfield));
    //     ASSERT(bitfield.offset == 5 && bitfield.width == 3);

    //     ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 0, &bitfield));
    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 1, &bitfield) == KEFIR_OUT_OF_SPACE);
    // } while (0);

    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_INT, 0, 0));
    // typeentry = kefir_ir_type_at(&type, 0);

    // do {
    //     struct kefir_ir_typeentry colocated = *typeentry;
        
    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 33, &bitfield) == KEFIR_OUT_OF_BOUNDS);
    //     ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 0, &bitfield));

    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 10, &bitfield));
    //     ASSERT(bitfield.offset == 0 && bitfield.width == 10);

    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 1, &bitfield));
    //     ASSERT(bitfield.offset == 10 && bitfield.width == 1);
    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 33, &bitfield) == KEFIR_OUT_OF_BOUNDS);

    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 3, &bitfield));
    //     ASSERT(bitfield.offset == 11 && bitfield.width == 3);
    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 34, &bitfield) == KEFIR_OUT_OF_BOUNDS);

    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 14, &bitfield));
    //     ASSERT(bitfield.offset == 14 && bitfield.width == 14);

    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 35, &bitfield) == KEFIR_OUT_OF_BOUNDS);
    //     ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 0, &bitfield));
    // } while (0);

    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_SHORT, 0, 0));
    // typeentry = kefir_ir_type_at(&type, 0);

    // do {
    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 17, &bitfield) == KEFIR_OUT_OF_BOUNDS);
    //     ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 0, &bitfield));

    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 16, &bitfield));
    //     ASSERT(bitfield.offset == 0 && bitfield.width == 16);

    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 17, &bitfield) == KEFIR_OUT_OF_BOUNDS);
    //     ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 0, &bitfield));
    // } while (0);

    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_LONG, 0, 0));
    // typeentry = kefir_ir_type_at(&type, 0);

    // do {
    //     struct kefir_ir_typeentry colocated = *typeentry;
        
    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 65, &bitfield) == KEFIR_OUT_OF_BOUNDS);
    //     ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 0, &bitfield));

    //     ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 1, &bitfield));
    //     ASSERT(bitfield.offset == 0 && bitfield.width == 1);
    //     for (kefir_size_t i = 1; i < 64; i++) {
    //         ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 1, &bitfield));
    //         ASSERT(bitfield.offset == i && bitfield.width == 1);
    //     }

    //     ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 70, &bitfield) == KEFIR_OUT_OF_BOUNDS);
    //     ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 0, &bitfield));
    // } while (0);

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    // ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_FREE(&kft_mem, &bitfields));
    // ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_FREE(&kft_mem, &platform));
END_CASE

DEFINE_CASE(amd64_sysv_abi_platform_bitfields2, "AMD64 System V ABI - platform bitfields #2")
    // struct kefir_ir_target_platform platform;
    // struct kefir_ir_bitfield_allocator bitfields;
    // struct kefir_ir_bitfield bitfield;
    // ASSERT_OK(kefir_codegen_amd64_sysv_target_platform(&platform));

    // struct kefir_ir_type type;
    // struct kefir_irbuilder_type builder;
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));
    // ASSERT_OK(kefir_irbuilder_type_init(&kft_mem, &builder, &type));

    // ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_BITFIELD_ALLOCATOR(&kft_mem, &platform, &type, &bitfields));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_STRUCT, 0, 0));
    // struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(&type, 0);
    // ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 1, &bitfield));
    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_ARRAY, 0, 0));
    // typeentry = kefir_ir_type_at(&type, 0);
    // ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 1, &bitfield));
    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_UNION, 0, 0));
    // typeentry = kefir_ir_type_at(&type, 0);
    // ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 1, &bitfield));
    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_MEMORY, 0, 0));
    // typeentry = kefir_ir_type_at(&type, 0);
    // ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 1, &bitfield));
    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    // typeentry = kefir_ir_type_at(&type, 0);
    // ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 1, &bitfield));
    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    // typeentry = kefir_ir_type_at(&type, 0);
    // ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 1, &bitfield));
    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, KEFIR_IR_TYPE_BUILTIN, 0, 0));
    // typeentry = kefir_ir_type_at(&type, 0);
    // ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, 1, &bitfield));
    // REQUIRE_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type));

    // ASSERT_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    // ASSERT_OK(kefir_ir_type_free(&kft_mem, &type));
    // ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_FREE(&kft_mem, &bitfields));
    // ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_FREE(&kft_mem, &platform));
END_CASE

DEFINE_CASE(amd64_sysv_abi_platform_bitfields3, "AMD64 System V ABI - platform bitfields #3")
//     struct kefir_ir_target_platform platform;
//     ASSERT_OK(kefir_codegen_amd64_sysv_target_platform(&platform));

// #define ASSERT_WIDTH(_typecode, _width) \
//     do { \
//         struct kefir_ir_bitfield_allocator bitfields; \
//         struct kefir_ir_bitfield bitfield; \
//         struct kefir_ir_type type; \
//         struct kefir_irbuilder_type builder; \
//         ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_BITFIELD_ALLOCATOR(&kft_mem, &platform, &type, &bitfields)); \
//          \
//         ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &type)); \
//         ASSERT_OK(kefir_irbuilder_type_init(&kft_mem, &builder, &type)); \
//  \
//         ASSERT_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(&builder, (_typecode), 0, 0)); \
//         struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(&type, 0); \
//         struct kefir_ir_typeentry colocated = *typeentry; \
//  \
//         ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&kft_mem, &bitfields, 0, typeentry, (_width), &bitfield)); \
//         ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(&kft_mem, &bitfields, &colocated, 1, &bitfield) == KEFIR_OUT_OF_SPACE); \
//  \
//         ASSERT_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder)); \
//         ASSERT_OK(kefir_ir_type_free(&kft_mem, &type)); \
//         ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_FREE(&kft_mem, &bitfields)); \
//     } while (0);

//     ASSERT_WIDTH(KEFIR_IR_TYPE_INT8, 8);
//     ASSERT_WIDTH(KEFIR_IR_TYPE_CHAR, 8);
//     ASSERT_WIDTH(KEFIR_IR_TYPE_BOOL, 8);
//     ASSERT_WIDTH(KEFIR_IR_TYPE_INT16, 16);
//     ASSERT_WIDTH(KEFIR_IR_TYPE_SHORT, 16);
//     ASSERT_WIDTH(KEFIR_IR_TYPE_INT32, 32);
//     ASSERT_WIDTH(KEFIR_IR_TYPE_INT, 32);
//     ASSERT_WIDTH(KEFIR_IR_TYPE_INT64, 64);
//     ASSERT_WIDTH(KEFIR_IR_TYPE_LONG, 64);
//     ASSERT_WIDTH(KEFIR_IR_TYPE_WORD, 64);

// #undef ASSERT_WIDTH

//     ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_FREE(&kft_mem, &platform));
END_CASE