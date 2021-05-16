#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_BITFIELDS_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_BITFIELDS_H_

#include "kefir/ir/bitfields.h"

kefir_result_t kefir_codegen_amd64_sysv_bitfield_allocator(struct kefir_mem *,
                                                       struct kefir_ir_type *,
                                                       struct kefir_ir_bitfield_allocator *);

#endif
