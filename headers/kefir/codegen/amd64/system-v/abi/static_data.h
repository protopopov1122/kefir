#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_STATIC_DATA_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_STATIC_DATA_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"
#include "kefir/codegen/amd64/asmgen.h"
#include "kefir/ir/data.h"

kefir_result_t kefir_amd64_sysv_static_data(struct kefir_mem *,
                                        struct kefir_codegen_amd64 *,
                                        const struct kefir_ir_data *,
                                        const char *);

#endif