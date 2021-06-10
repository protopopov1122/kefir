#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_INSTR_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_INSTR_H_

#include "kefir/codegen/codegen.h"
#include "kefir/codegen/amd64/system-v/abi.h"

kefir_result_t kefir_amd64_sysv_instruction(struct kefir_mem *, struct kefir_codegen_amd64 *,
                                            struct kefir_amd64_sysv_function *,
                                            struct kefir_codegen_amd64_sysv_module *sysv_module,
                                            const struct kefir_irinstr *);

#endif
