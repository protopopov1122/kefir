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
    REQUIRE_OK(kefir_ir_type_appendi64(type, typeentry));
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

kefir_result_t kefir_irbuilder_block_appendi64(struct kefir_mem *mem,
                                        struct kefir_irblock *block,
                                        kefir_iropcode_t opcode,
                                        kefir_int64_t arg) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_appendi64(block, opcode, arg));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_appendu64(struct kefir_mem *mem,
                                        struct kefir_irblock *block,
                                        kefir_iropcode_t opcode,
                                        kefir_uint64_t arg) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_appendu64(block, opcode, arg));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_appendi32(struct kefir_mem *mem,
                                         struct kefir_irblock *block,
                                         kefir_iropcode_t opcode, 
                                         kefir_int32_t arg1,
                                         kefir_int32_t arg2) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_appendi32(block, opcode, arg1, arg2));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_appendu32(struct kefir_mem *mem,
                                         struct kefir_irblock *block,
                                         kefir_iropcode_t opcode, 
                                         kefir_uint32_t arg1,
                                         kefir_uint32_t arg2) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_appendu32(block, opcode, arg1, arg2));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_appendf64(struct kefir_mem *mem,
                                         struct kefir_irblock *block, 
                                         kefir_iropcode_t opcode,
                                         double arg) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_appendf64(block, opcode, arg));
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_appendf32(struct kefir_mem *mem,
                                          struct kefir_irblock *block,
                                          kefir_iropcode_t opcode,
                                          float arg1,
                                          float arg2) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Epected valid IR block"));
    if (kefir_irblock_available(block) == 0) {
        REQUIRE_OK(kefir_irblock_realloc(mem, GROW(kefir_irblock_length(block)), block));
    }
    REQUIRE_OK(kefir_irblock_appendf32(block, opcode, arg1, arg2));
    return KEFIR_OK;
}

static kefir_result_t block_builder_appendi64(struct kefir_irbuilder_block *builder,
                                         kefir_iropcode_t opcode,
                                         kefir_int64_t arg) {
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    return kefir_irbuilder_block_appendi64(builder->payload, builder->block, opcode, arg);
}

static kefir_result_t block_builder_appendu64(struct kefir_irbuilder_block *builder,
                                         kefir_iropcode_t opcode,
                                         kefir_uint64_t arg) {
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    return kefir_irbuilder_block_appendu64(builder->payload, builder->block, opcode, arg);
}

static kefir_result_t block_builder_appendi32(struct kefir_irbuilder_block *builder,
                                          kefir_iropcode_t opcode,
                                          kefir_int32_t arg1,
                                          kefir_int32_t arg2) {
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    return kefir_irbuilder_block_appendi32(builder->payload, builder->block, opcode, arg1, arg2);
}

static kefir_result_t block_builder_appendu32(struct kefir_irbuilder_block *builder,
                                          kefir_iropcode_t opcode,
                                          kefir_uint32_t arg1,
                                          kefir_uint32_t arg2) {
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    return kefir_irbuilder_block_appendu32(builder->payload, builder->block, opcode, arg1, arg2);
}

static kefir_result_t block_builder_appendf64(struct kefir_irbuilder_block *builder,
                                          kefir_iropcode_t opcode,
                                          kefir_float64_t arg) {
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    return kefir_irbuilder_block_appendf64(builder->payload, builder->block, opcode, arg);
}

static kefir_result_t block_builder_appendf32(struct kefir_irbuilder_block *builder, 
                                           kefir_iropcode_t opcode,
                                           kefir_float32_t arg1,
                                           kefir_float32_t arg2) {
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    return kefir_irbuilder_block_appendf32(builder->payload, builder->block, opcode, arg1, arg2);
}

static kefir_result_t block_builder_free(struct kefir_irbuilder_block *builder) {
    UNUSED(builder);
    return KEFIR_OK;
}

kefir_result_t kefir_irbuilder_block_init(struct kefir_mem *mem,
                                      struct kefir_irbuilder_block *builder,
                                      struct kefir_irblock *block) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block"));
    builder->block = block;
    builder->payload = mem;
    builder->appendi64 = block_builder_appendi64;
    builder->appendu64 = block_builder_appendu64;
    builder->appendi32 = block_builder_appendi32;
    builder->appendu32 = block_builder_appendu32;
    builder->appendf64 = block_builder_appendf64;
    builder->appendf32 = block_builder_appendf32;
    builder->free = block_builder_free;
    return KEFIR_OK;
}