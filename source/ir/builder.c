#include "kefir/ir/builder.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"

#define GROW(x) (3 * (x) / 2 + 5)

kefir_result_t kefir_irbuilder_type_append(struct kefir_mem *mem,
                                       struct kefir_ir_type *type,
                                       const struct kefir_ir_typeentry *typeentry) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR type"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type entry"));
    if (kefir_ir_type_raw_available(type) == 0) {
        REQUIRE_OK(kefir_ir_type_realloc(mem, GROW(kefir_ir_type_raw_length(type)), type));
    }
    REQUIRE_OK(kefir_ir_type_append(type, typeentry));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_type_append_v(struct kefir_mem *mem,
                                         struct kefir_ir_type *type,
                                         kefir_ir_typecode_t typecode,
                                         kefir_uint32_t alignment,
                                         kefir_int64_t arg) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR type"));
    if (kefir_ir_type_raw_available(type) == 0) {
        REQUIRE_OK(kefir_ir_type_realloc(mem, GROW(kefir_ir_type_raw_length(type)), type));
    }
    REQUIRE_OK(kefir_ir_type_append_v(type, typecode, alignment, arg));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_type_append_e(struct kefir_mem *mem,
                                         struct kefir_ir_type *type,
                                         const struct kefir_ir_type *source,
                                         kefir_size_t length) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR type"));
    REQUIRE(source != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid source IR type"));
    if (kefir_ir_type_raw_available(type) < length) {
        REQUIRE_OK(kefir_ir_type_realloc(mem, GROW(kefir_ir_type_raw_length(type) + length), type));
    }
    REQUIRE_OK(kefir_ir_type_append_e(type, source, length));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_append(struct kefir_mem *mem,
                                        struct kefir_irblock *block,
                                        kefir_iropcode_t opcode,
                                        kefir_int64_t arg) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_append(block, opcode, arg));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_append2(struct kefir_mem *mem,
                                         struct kefir_irblock *block,
                                         kefir_iropcode_t opcode, 
                                         kefir_uint32_t arg1,
                                         kefir_uint32_t arg2) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_append2(block, opcode, arg1, arg2));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_appendf(struct kefir_mem *mem,
                                         struct kefir_irblock *block, 
                                         kefir_iropcode_t opcode,
                                         double arg) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_appendf(block, opcode, arg));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_appendf2(struct kefir_mem *mem,
                                          struct kefir_irblock *block,
                                          kefir_iropcode_t opcode,
                                          float arg1,
                                          float arg2) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_appendf2(block, opcode, arg1, arg2));
    return KEFIR_OK;
}