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

kefir_result_t kefir_irbuilder_block_append(struct kefir_mem *,
                                        struct kefir_irblock *,
                                        kefir_iropcode_t,
                                        kefir_int64_t);
kefir_result_t kefir_irbuilder_block_append2(struct kefir_mem *,
                                         struct kefir_irblock *,
                                         kefir_iropcode_t, 
                                         kefir_uint32_t,
                                         kefir_uint32_t);
kefir_result_t kefir_irbuilder_block_appendf(struct kefir_mem *,
                                         struct kefir_irblock *, 
                                         kefir_iropcode_t,
                                         double);
kefir_result_t kefir_irbuilder_block_appendf2(struct kefir_mem *,
                                          struct kefir_irblock *,
                                          kefir_iropcode_t,
                                          float,
                                          float);

#endif