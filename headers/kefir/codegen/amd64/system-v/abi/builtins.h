#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_BUILTINS_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_BUILTINS_H_

#include "kefir/ir/builtins.h"
#include "kefir/codegen/amd64/system-v/abi/data.h"
#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64-sysv.h"

typedef struct kefir_codegen_amd64_sysv_builtin_type {
    kefir_result_t (*layout)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                           const struct kefir_ir_typeentry *,
                           struct kefir_amd64_sysv_data_layout *);
    kefir_result_t (*classify_nested)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                    const struct kefir_ir_typeentry *,
                                    struct kefir_amd64_sysv_parameter_allocation *immediate_allocation,
                                    struct kefir_amd64_sysv_parameter_allocation *allocation);
    kefir_result_t (*classify_immediate)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                       const struct kefir_ir_typeentry *,
                                       struct kefir_amd64_sysv_parameter_allocation *);
    kefir_result_t (*allocate_immediate)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                       const struct kefir_ir_typeentry *,
                                       struct kefir_amd64_sysv_parameter_location *total_allocation,
                                       struct kefir_amd64_sysv_parameter_allocation *allocation);
    kefir_result_t (*load_argument)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                  const struct kefir_ir_typeentry *,
                                  struct kefir_codegen_amd64 *,
                                  struct kefir_amd64_sysv_parameter_allocation *);
    kefir_result_t (*return_value)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                  const struct kefir_ir_typeentry *,
                                  struct kefir_codegen_amd64 *,
                                  struct kefir_amd64_sysv_parameter_allocation *);
    kefir_result_t (*store_argument)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                   const struct kefir_ir_typeentry *,
                                   struct kefir_codegen_amd64 *,
                                   struct kefir_amd64_sysv_parameter_allocation *,
                                   kefir_size_t);
    kefir_result_t (*load_return_value)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                  const struct kefir_ir_typeentry *,
                                  struct kefir_codegen_amd64 *,
                                  struct kefir_amd64_sysv_parameter_allocation *);
} kefir_codegen_amd64_sysv_builtin_type_t;

extern const struct kefir_codegen_amd64_sysv_builtin_type KEFIR_CODEGEN_AMD64_SYSV_BUILTIN_TYPES[];

#endif