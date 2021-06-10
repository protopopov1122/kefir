#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_MODULE_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_MODULE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/codegen/codegen.h"
#include "kefir/codegen/amd64/asmgen.h"
#include "kefir/core/hashtree.h"

typedef struct kefir_codegen_amd64_sysv_module {
    const struct kefir_ir_module *module;
    struct kefir_hashtree function_gates;
    struct kefir_hashtree function_vgates;
    struct kefir_hashtree type_layouts;
} kefir_codegen_amd64_sysv_module_t;

kefir_result_t kefir_codegen_amd64_sysv_module_alloc(struct kefir_mem *, struct kefir_codegen_amd64_sysv_module *,
                                                     const struct kefir_ir_module *);
kefir_result_t kefir_codegen_amd64_sysv_module_free(struct kefir_mem *, struct kefir_codegen_amd64_sysv_module *);
struct kefir_amd64_sysv_function_decl *kefir_codegen_amd64_sysv_module_function_decl(
    struct kefir_mem *, struct kefir_codegen_amd64_sysv_module *, kefir_id_t, bool);
struct kefir_vector *kefir_codegen_amd64_sysv_module_type_layout(struct kefir_mem *,
                                                                 struct kefir_codegen_amd64_sysv_module *, kefir_id_t);

#endif
