#ifndef KEFIR_IR_BUILDER_H_
#define KEFIR_IR_BUILDER_H_

#include "kefir/core/mem.h"
#include "kefir/ir/type.h"
#include "kefir/ir/instr.h"

kefir_result_t kefir_irbuilder_type_append(struct kefir_mem *,
                                       struct kefir_ir_type *,
                                       const struct kefir_ir_typeentry *);
kefir_result_t kefir_irbuilder_type_append_v(struct kefir_mem *,
                                         struct kefir_ir_type *,
                                         kefir_ir_typecode_t,
                                         kefir_uint32_t,
                                         kefir_int64_t);
kefir_result_t kefir_irbuilder_type_append_e(struct kefir_mem *,
                                         struct kefir_ir_type *,
                                         const struct kefir_ir_type *,
                                         kefir_size_t);

kefir_result_t kefir_irbuilder_block_appendi64(struct kefir_mem *,
                                        struct kefir_irblock *,
                                        kefir_iropcode_t,
                                        kefir_int64_t);
kefir_result_t kefir_irbuilder_block_appendu64(struct kefir_mem *,
                                        struct kefir_irblock *,
                                        kefir_iropcode_t,
                                        kefir_uint64_t);
kefir_result_t kefir_irbuilder_block_appendi32(struct kefir_mem *,
                                         struct kefir_irblock *,
                                         kefir_iropcode_t, 
                                         kefir_int32_t,
                                         kefir_int32_t);
kefir_result_t kefir_irbuilder_block_appendu32(struct kefir_mem *,
                                         struct kefir_irblock *,
                                         kefir_iropcode_t, 
                                         kefir_uint32_t,
                                         kefir_uint32_t);
kefir_result_t kefir_irbuilder_block_appendf64(struct kefir_mem *,
                                         struct kefir_irblock *, 
                                         kefir_iropcode_t,
                                         double);
kefir_result_t kefir_irbuilder_block_appendf32(struct kefir_mem *,
                                          struct kefir_irblock *,
                                          kefir_iropcode_t,
                                          float,
                                          float);

typedef struct kefir_irbuilder_block {
    struct kefir_irblock *block;
    void *payload;

    kefir_result_t (*appendi64)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_int64_t);
    kefir_result_t (*appendu64)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_uint64_t);
    kefir_result_t (*appendi32)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_int32_t, kefir_int32_t);
    kefir_result_t (*appendu32)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_uint32_t, kefir_uint32_t);
    kefir_result_t (*appendf64)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_float64_t);
    kefir_result_t (*appendf32)(struct kefir_irbuilder_block *, kefir_iropcode_t, kefir_float32_t, kefir_float32_t);
    kefir_result_t (*free)(struct kefir_irbuilder_block *);
} kefir_irbuilder_block_t;

kefir_result_t kefir_irbuilder_block_init(struct kefir_mem *,
                                      struct kefir_irbuilder_block *,
                                      struct kefir_irblock *);

#define KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, opcode, arg) \
    ((builder)->appendi64((builder), (opcode), (arg)))
#define KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, opcode, arg) \
    ((builder)->appendu64((builder), (opcode), (arg)))
#define KEFIR_IRBUILDER_BLOCK_APPENDI32(builder, opcode, arg1, arg2) \
    ((builder)->appendi32((builder), (opcode), (arg1), (arg2)))
#define KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, opcode, arg1, arg2) \
    ((builder)->appendu32((builder), (opcode), (arg1), (arg2)))
#define KEFIR_IRBUILDER_BLOCK_APPENDF64(builder, opcode, arg) \
    ((builder)->appendf64((builder), (opcode), (arg)))
#define KEFIR_IRBUILDER_BLOCK_APPENDF32(builder, opcode, arg1, arg2) \
    ((builder)->appendf32((builder), (opcode), (arg1), (arg2)))
#define KEFIR_IRBUILDER_BLOCK_FREE(builder) \
    ((builder)->free((builder)))

#endif