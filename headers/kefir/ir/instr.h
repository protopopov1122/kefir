#ifndef KEFIR_IR_INSTR_H_
#define KEFIR_IR_INSTR_H_

#include "kefir/core/basic-types.h"
#include "kefir/ir/opcodes.h"
#include "kefir/core/vector_util.h"
#include "kefir/core/mem.h"

typedef struct kefir_irinstr {
    kefir_iropcode_t opcode;
    kefir_int64_t arg;
} kefir_irinstr_t;

typedef KEFIR_VECTOR_STRUCT(kefir_irblock, struct kefir_irinstr, code) kefir_irblock_t;

kefir_result_t kefir_irblock_init(struct kefir_irblock *, void *, kefir_size_t);
kefir_size_t kefir_irblock_available(const struct kefir_irblock *);
kefir_size_t kefir_irblock_length(const struct kefir_irblock *);
const struct kefir_irinstr *kefir_irblock_at(const struct kefir_irblock *, kefir_size_t);
kefir_result_t kefir_irblock_append(struct kefir_irblock *, kefir_iropcode_t, kefir_int64_t);
kefir_result_t kefir_irblock_copy(struct kefir_irblock *, const struct kefir_irblock *);
kefir_result_t kefir_irblock_alloc(struct kefir_mem *, kefir_size_t, struct kefir_irblock *);
kefir_result_t kefir_irblock_realloc(struct kefir_mem *, kefir_size_t, struct kefir_irblock *);
kefir_result_t kefir_irblock_free(struct kefir_mem *, struct kefir_irblock *);

#endif