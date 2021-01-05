#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"
#include "kefir/codegen/amd64/asmgen.h"
#include "kefir/ir/function.h"
#include "kefir/ir/data.h"

typedef enum kefir_amd64_sysv_internal_fields {
    KEFIR_AMD64_SYSV_INTERNAL_RETURN_ADDRESS = 0,
    // Auxilary
    KEFIR_AMD64_SYSV_INTERNAL_COUNT
} kefir_amd64_sysv_internal_fields_t;

typedef struct kefir_amd64_sysv_function {
    const struct kefir_ir_function *func;
    struct {
        struct kefir_vector layout;
        struct kefir_vector allocation;
        struct kefir_amd64_sysv_parameter_location location;
    } parameters;

    struct {
        struct kefir_vector layout;
        struct kefir_vector allocation;
    } returns;

    struct {
        kefir_size_t size;
        kefir_size_t alignment;
        struct {
            kefir_size_t internals;
            kefir_size_t parameters;
            kefir_size_t locals;
        } base;
    } frame;

    struct {
        bool enabled;
        kefir_size_t offset;
    } internals[KEFIR_AMD64_SYSV_INTERNAL_COUNT];
} kefir_amd64_sysv_function_t;

kefir_result_t kefir_amd64_sysv_function_alloc(struct kefir_mem *, const struct kefir_ir_function *, struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_free(struct kefir_mem *, struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_prologue(struct kefir_codegen_amd64 *, const struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_epilogue(struct kefir_codegen_amd64 *, const struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_static_data(struct kefir_mem *, struct kefir_codegen_amd64 *, const struct kefir_ir_data *, const char *);

#define KEFIR_AMD64_SYSV_ABI_PROGRAM_REG KEFIR_AMD64_RBX
#define KEFIR_AMD64_SYSV_ABI_DATA_REG KEFIR_AMD64_R11
#define KEFIR_AMD64_SYSV_ABI_DATA2_REG KEFIR_AMD64_R12
#define KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG KEFIR_AMD64_R13

#endif