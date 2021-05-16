#ifndef KEFIR_IR_INSTR_H_
#define KEFIR_IR_INSTR_H_

#include "kefir/core/basic-types.h"
#include "kefir/ir/opcodes.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"

typedef struct kefir_irinstr {
    kefir_iropcode_t opcode;
    union {
        kefir_int64_t i64;
        kefir_uint64_t u64;
        kefir_int32_t i32[2];
        kefir_uint32_t u32[2];
        kefir_float64_t f64;
        kefir_float32_t f32[2];
    } arg;
} kefir_irinstr_t;

typedef struct kefir_irblock {
    struct kefir_vector content;
} kefir_irblock_t;

kefir_result_t kefir_irblock_init(struct kefir_irblock *, void *, kefir_size_t);
kefir_size_t kefir_irblock_available(const struct kefir_irblock *);
kefir_size_t kefir_irblock_length(const struct kefir_irblock *);
struct kefir_irinstr *kefir_irblock_at(const struct kefir_irblock *, kefir_size_t);
kefir_result_t kefir_irblock_appendi64(struct kefir_irblock *, kefir_iropcode_t, kefir_int64_t);
kefir_result_t kefir_irblock_appendu64(struct kefir_irblock *, kefir_iropcode_t, kefir_uint64_t);
kefir_result_t kefir_irblock_appendi32(struct kefir_irblock *, kefir_iropcode_t, kefir_int32_t, kefir_int32_t);
kefir_result_t kefir_irblock_appendu32(struct kefir_irblock *, kefir_iropcode_t, kefir_uint32_t, kefir_uint32_t);
kefir_result_t kefir_irblock_appendf64(struct kefir_irblock *, kefir_iropcode_t, kefir_float64_t);
kefir_result_t kefir_irblock_appendf32(struct kefir_irblock *, kefir_iropcode_t, kefir_float32_t, kefir_float32_t);
kefir_result_t kefir_irblock_copy(struct kefir_irblock *, const struct kefir_irblock *);
kefir_result_t kefir_irblock_alloc(struct kefir_mem *, kefir_size_t, struct kefir_irblock *);
kefir_result_t kefir_irblock_realloc(struct kefir_mem *, kefir_size_t, struct kefir_irblock *);
kefir_result_t kefir_irblock_free(struct kefir_mem *, struct kefir_irblock *);

#endif
