#include "kefir/test/unit_test.h"
#include "kefir/codegen/amd64/system-v/platform.h"

DEFINE_CASE(amd64_sysv_abi_platform_bitfields1, "AMD64 System V ABI - platform bitfields #1")
    struct kefir_ir_target_platform platform;
    struct kefir_ir_bitfield_allocator bitfields;
    ASSERT_OK(kefir_codegen_amd64_sysv_target_platform(&platform));
    ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_BITFIELD_ALLOCATOR(&kft_mem, &platform, &bitfields));

    struct kefir_ir_bitfield bitfield;
    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 0, &bitfield));
    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield));
    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 8, &bitfield));

    do {
        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_INT8));
        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 9, &bitfield) == KEFIR_OUT_OF_BOUNDS);
        ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 0, &bitfield));

        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield));
        ASSERT(bitfield.offset == 0 && bitfield.width == 1);
        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield));
        ASSERT(bitfield.offset == 1 && bitfield.width == 1);
        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield));
        ASSERT(bitfield.offset == 2 && bitfield.width == 1);

        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 2, &bitfield));
        ASSERT(bitfield.offset == 3 && bitfield.width == 2);
        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 3, &bitfield));
        ASSERT(bitfield.offset == 5 && bitfield.width == 3);

        ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 0, &bitfield));
        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield) == KEFIR_OUT_OF_SPACE);
        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 2, &bitfield) == KEFIR_OUT_OF_SPACE);
        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 5, &bitfield) == KEFIR_OUT_OF_SPACE);
    } while (0);

    do {
        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_INT32));
        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 33, &bitfield) == KEFIR_OUT_OF_BOUNDS);
        ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 0, &bitfield));

        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 10, &bitfield));
        ASSERT(bitfield.offset == 0 && bitfield.width == 10);

        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield));
        ASSERT(bitfield.offset == 10 && bitfield.width == 1);
        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 22, &bitfield) == KEFIR_OUT_OF_SPACE);

        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 3, &bitfield));
        ASSERT(bitfield.offset == 11 && bitfield.width == 3);
        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 19, &bitfield) == KEFIR_OUT_OF_SPACE);

        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 18, &bitfield));
        ASSERT(bitfield.offset == 14 && bitfield.width == 18);

        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield) == KEFIR_OUT_OF_SPACE);
        ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 0, &bitfield));
    } while (0);
    
    do {
        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_INT16));
        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 17, &bitfield) == KEFIR_OUT_OF_BOUNDS);
        ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 0, &bitfield));

        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 16, &bitfield));
        ASSERT(bitfield.offset == 0 && bitfield.width == 16);

        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield) == KEFIR_OUT_OF_SPACE);
        ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 0, &bitfield));
    } while (0);

    do {
        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_INT64));
        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 65, &bitfield) == KEFIR_OUT_OF_BOUNDS);
        ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 0, &bitfield));

        for (kefir_size_t i = 0; i < 64; i++) {
            ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield));
            ASSERT(bitfield.offset == i && bitfield.width == 1);
        }

        ASSERT(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield) == KEFIR_OUT_OF_SPACE);
        ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 0, &bitfield));
    } while (0);

    ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_FREE(&kft_mem, &bitfields));
    ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_FREE(&kft_mem, &platform));
END_CASE

DEFINE_CASE(amd64_sysv_abi_platform_bitfields2, "AMD64 System V ABI - platform bitfields #2")
    struct kefir_ir_target_platform platform;
    struct kefir_ir_bitfield_allocator bitfields;
    ASSERT_OK(kefir_codegen_amd64_sysv_target_platform(&platform));
    ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_BITFIELD_ALLOCATOR(&kft_mem, &platform, &bitfields));

    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_STRUCT));
    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_ARRAY));
    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_UNION));
    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_MEMORY));
    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_FLOAT32));
    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_FLOAT64));
    ASSERT_NOK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, KEFIR_IR_TYPE_BUILTIN));

    ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_FREE(&kft_mem, &bitfields));
    ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_FREE(&kft_mem, &platform));
END_CASE

DEFINE_CASE(amd64_sysv_abi_platform_bitfields3, "AMD64 System V ABI - platform bitfields #3")
    struct kefir_ir_target_platform platform;
    struct kefir_ir_bitfield_allocator bitfields;
    struct kefir_ir_bitfield bitfield;
    ASSERT_OK(kefir_codegen_amd64_sysv_target_platform(&platform));
    ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_BITFIELD_ALLOCATOR(&kft_mem, &platform, &bitfields));

#define ASSERT_WIDTH(_typecode, _width) \
    do { \
        ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_INIT(&bitfields, (_typecode))); \
        kefir_result_t res = KEFIR_OK; \
        kefir_size_t counter = 0; \
        while (res == KEFIR_OK) { \
            res = KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(&bitfields, 1, &bitfield); \
            if (res == KEFIR_OK) { \
                ASSERT(bitfield.offset == counter && bitfield.width == 1); \
                counter++; \
            } \
        } \
        ASSERT(res == KEFIR_OUT_OF_SPACE); \
        ASSERT(counter == (_width)); \
    } while (0)

    ASSERT_WIDTH(KEFIR_IR_TYPE_INT8, 8);
    ASSERT_WIDTH(KEFIR_IR_TYPE_CHAR, 8);
    ASSERT_WIDTH(KEFIR_IR_TYPE_BOOL, 8);
    ASSERT_WIDTH(KEFIR_IR_TYPE_INT16, 16);
    ASSERT_WIDTH(KEFIR_IR_TYPE_SHORT, 16);
    ASSERT_WIDTH(KEFIR_IR_TYPE_INT32, 32);
    ASSERT_WIDTH(KEFIR_IR_TYPE_INT, 32);
    ASSERT_WIDTH(KEFIR_IR_TYPE_INT64, 64);
    ASSERT_WIDTH(KEFIR_IR_TYPE_LONG, 64);
    ASSERT_WIDTH(KEFIR_IR_TYPE_WORD, 64);

#undef ASSERT_WIDTH

    ASSERT_OK(KEFIR_IR_BITFIELD_ALLOCATOR_FREE(&kft_mem, &bitfields));
    ASSERT_OK(KEFIR_IR_TARGET_PLATFORM_FREE(&kft_mem, &platform));
END_CASE