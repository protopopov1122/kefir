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
    KEFIR_AMD64_SYSV_INTERNAL_COUNT = 2
} kefir_amd64_sysv_internal_fields_t;

_Static_assert(KEFIR_AMD64_SYSV_INTERNAL_COUNT % 2 == 0, "KEFIR_AMD64_SYSV_INTERNAL_COUNT must be divisible by 2");

#define KEFIR_AMD64_SYSV_INTERNAL_BOUND (KEFIR_AMD64_SYSV_INTERNAL_COUNT * KEFIR_AMD64_SYSV_ABI_QWORD)

typedef struct kefir_amd64_sysv_function_decl {
    const struct kefir_ir_function_decl *decl;
    struct {
        struct kefir_vector layout;
        struct kefir_vector allocation;
        struct kefir_amd64_sysv_parameter_location location;
    } parameters;

    struct {
        struct kefir_vector layout;
        struct kefir_vector allocation;
        bool implicit_parameter;
    } returns;
} kefir_amd64_sysv_function_decl_t;


typedef enum kefir_amd64_sysv_appendix_id {
    KEFIR_AMD64_SYSV_APPENDIX_VARARG_START = 0,
    KEFIR_AMD64_SYSV_APPENDIX_VARARG_END,
    KEFIR_AMD64_SYSV_APPENDIX_CUSTOM,
} kefir_amd64_sysv_appendix_id_t;

typedef struct kefir_amd64_sysv_function {
    const struct kefir_ir_function *func;
    struct kefir_amd64_sysv_function_decl decl;
    struct kefir_vector local_layout;
    struct kefir_hashtree appendix;
    kefir_size_t appendix_index;

    struct {
        kefir_size_t size;
        kefir_size_t alignment;
        struct {
            kefir_size_t internals;
            kefir_size_t temporary;
            kefir_size_t parameters;
            kefir_size_t register_save_area;
            kefir_size_t locals;
        } base;
    } frame;
} kefir_amd64_sysv_function_t;

typedef kefir_result_t (*kefir_amd64_sysv_function_appendix_t)(struct kefir_codegen_amd64 *,
                                                           struct kefir_codegen_amd64_sysv_module *,
                                                           const struct kefir_amd64_sysv_function *,
                                                           kefir_size_t,
                                                           void *);

typedef struct kefir_amd64_sysv_appendix_data {
    kefir_amd64_sysv_function_appendix_t callback;
    kefir_result_t (*cleanup)(struct kefir_mem *, void *);
    void *payload;
} kefir_amd64_sysv_appendix_data_t;

kefir_result_t kefir_amd64_sysv_function_decl_alloc(struct kefir_mem *,
                                                const struct kefir_ir_function_decl *,
                                                struct kefir_amd64_sysv_function_decl *);
kefir_result_t kefir_amd64_sysv_function_decl_free(struct kefir_mem *,
                                               struct kefir_amd64_sysv_function_decl *);
kefir_result_t kefir_amd64_sysv_function_alloc(struct kefir_mem *,
                                           struct kefir_codegen_amd64_sysv_module*,
                                           const struct kefir_ir_function *,
                                           struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_free(struct kefir_mem *, struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_has_appendix(struct kefir_amd64_sysv_function *,
                                                  kefir_size_t);
kefir_result_t kefir_amd64_sysv_function_insert_appendix(struct kefir_mem *,
                                                     struct kefir_amd64_sysv_function *,
                                                     kefir_amd64_sysv_function_appendix_t,
                                                     kefir_result_t (*)(struct kefir_mem *, void *),
                                                     void *,
                                                     kefir_size_t);
kefir_result_t kefir_amd64_sysv_function_prologue(struct kefir_codegen_amd64 *, const struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_epilogue(struct kefir_codegen_amd64 *, const struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_static_data(struct kefir_mem *, struct kefir_codegen_amd64 *, const struct kefir_ir_data *, const char *);
kefir_result_t kefir_amd64_sysv_function_invoke(struct kefir_codegen_amd64 *,
                                            const struct kefir_amd64_sysv_function_decl *,
                                            bool);

#define KEFIR_AMD64_SYSV_ABI_PROGRAM_REG KEFIR_AMD64_RBX
#define KEFIR_AMD64_SYSV_ABI_TMP_REG KEFIR_AMD64_R11
#define KEFIR_AMD64_SYSV_ABI_DATA_REG KEFIR_AMD64_R12
#define KEFIR_AMD64_SYSV_ABI_DATA2_REG KEFIR_AMD64_R13
#define KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG KEFIR_AMD64_R14

#endif