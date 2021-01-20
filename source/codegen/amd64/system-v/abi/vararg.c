#include "kefir/codegen/amd64/system-v/abi/vararg.h"
#include "kefir/codegen/amd64/system-v/abi/qwords.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/labels.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/util.h"
#include "kefir/core/error.h"

static kefir_result_t vararg_start(struct kefir_codegen_amd64 *codegen,
                                 struct kefir_codegen_amd64_sysv_module *sysv_module,
                                 const struct kefir_amd64_sysv_function *sysv_func,
                                 kefir_size_t appendix_id,
                                 void *payload) {
    UNUSED(sysv_module);
    UNUSED(appendix_id);
    UNUSED(payload);
    ASMGEN_LABEL(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_START_LABEL,
        sysv_func->func->declaration->identifier);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_QWORD KEFIR_AMD64_INDIRECT,
        KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        KEFIR_AMD64_SYSV_ABI_QWORD * sysv_func->decl.parameters.location.integer_register);
    
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_QWORD KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        KEFIR_AMD64_SYSV_ABI_QWORD * KEFIR_AMD64_SYSV_INTEGER_REGISTER_COUNT +
        2 * KEFIR_AMD64_SYSV_ABI_QWORD * sysv_func->decl.parameters.location.sse_register);
    
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_RBP,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD + sysv_func->decl.parameters.location.stack_offset);
    
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
        sysv_func->frame.base.register_save_area);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        3 * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RBX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_RBX);
    return KEFIR_OK;
}

static kefir_result_t vararg_end(struct kefir_codegen_amd64 *codegen,
                                 struct kefir_codegen_amd64_sysv_module *sysv_module,
                                 const struct kefir_amd64_sysv_function *sysv_func,
                                 kefir_size_t appendix_id,
                                 void *payload) {
    UNUSED(sysv_module);
    UNUSED(appendix_id);
    UNUSED(payload);
    ASMGEN_LABEL(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_END_LABEL,
        sysv_func->func->declaration->identifier);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RBX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_RBX);
    return KEFIR_OK;
}

struct vararg_getter {
    struct kefir_codegen_amd64 *codegen;
    struct kefir_codegen_amd64_sysv_module *sysv_module;
    struct kefir_amd64_sysv_function *sysv_func;
};

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type,
                                          kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry,
                                          void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Encountered not supported type code while traversing type");
}

static kefir_result_t vararg_get_int(struct kefir_codegen_amd64 *codegen,
                                 struct kefir_codegen_amd64_sysv_module *sysv_module,
                                 const struct kefir_amd64_sysv_function *sysv_func,
                                 kefir_size_t appendix_id,
                                 void *payload) {
    UNUSED(sysv_module);
    UNUSED(payload);
    ASMGEN_LABEL(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL,
        sysv_func->func->declaration->identifier,
        appendix_id);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CALL);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_VARARG_INT);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RBX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_RBX);
    return KEFIR_OK;
}

static kefir_result_t vararg_visit_integer(const struct kefir_ir_type *type,
                                          kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry,
                                          void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct vararg_getter *, param,
        payload);
    kefir_result_t res = kefir_amd64_sysv_function_insert_appendix(param->codegen->mem, param->sysv_func,
        vararg_get_int, NULL, NULL, KEFIR_AMD64_SYSV_APPENDIX_VARARG_GET_INTEGER);
    REQUIRE(res == KEFIR_OK || res == KEFIR_ALREADY_EXISTS, res);
    ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
    ASMGEN_ARG(&param->codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL,
        param->sysv_func->func->declaration->identifier,
        KEFIR_AMD64_SYSV_APPENDIX_VARARG_GET_INTEGER);
    ASMGEN_ARG0(&param->codegen->asmgen, "0");
    return KEFIR_OK;
}

static kefir_result_t vararg_get_sse(struct kefir_codegen_amd64 *codegen,
                                 struct kefir_codegen_amd64_sysv_module *sysv_module,
                                 const struct kefir_amd64_sysv_function *sysv_func,
                                 kefir_size_t appendix_id,
                                 void *payload) {
    UNUSED(sysv_module);
    UNUSED(payload);
    ASMGEN_LABEL(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL,
        sysv_func->func->declaration->identifier,
        appendix_id);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CALL);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_VARARG_SSE);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RBX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_RBX);
    return KEFIR_OK;
}

static kefir_result_t vararg_visit_sse(const struct kefir_ir_type *type,
                                          kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry,
                                          void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct vararg_getter *, param,
        payload);
    kefir_result_t res = kefir_amd64_sysv_function_insert_appendix(param->codegen->mem, param->sysv_func,
        vararg_get_sse, NULL, NULL, KEFIR_AMD64_SYSV_APPENDIX_VARARG_GET_SSE);
    REQUIRE(res == KEFIR_OK || res == KEFIR_ALREADY_EXISTS, res);
    ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
    ASMGEN_ARG(&param->codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL,
        param->sysv_func->func->declaration->identifier,
        KEFIR_AMD64_SYSV_APPENDIX_VARARG_GET_SSE);
    ASMGEN_ARG0(&param->codegen->asmgen, "0");
    return KEFIR_OK;
}

struct vararg_aggregate_info {
    struct kefir_amd64_sysv_data_layout *arg_layout;
    struct kefir_amd64_sysv_parameter_allocation *arg_alloc;

    struct kefir_vector layout;
    struct kefir_vector allocation;
};

static kefir_result_t vararg_load_memory_aggregate(struct kefir_codegen_amd64 *codegen,
                                                 struct vararg_aggregate_info *info) {
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD);
    if (info->arg_layout->alignment > KEFIR_AMD64_SYSV_ABI_QWORD) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
        ASMGEN_ARG(&codegen->asmgen,
            KEFIR_INT64_FMT,
            info->arg_layout->alignment - 1);
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_AND);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
        ASMGEN_ARG(&codegen->asmgen,
            KEFIR_INT64_FMT,
            -((kefir_int64_t) info->arg_layout->alignment));
    }
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        kefir_codegen_pad_aligned(info->arg_layout->size, KEFIR_AMD64_SYSV_ABI_QWORD));

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    return KEFIR_OK;
}

static kefir_result_t vararg_get_memory_aggregate(struct kefir_codegen_amd64 *codegen,
                                 struct kefir_codegen_amd64_sysv_module *sysv_module,
                                 const struct kefir_amd64_sysv_function *sysv_func,
                                 kefir_size_t appendix_id,
                                 void *payload) {
    UNUSED(sysv_module);
    ASSIGN_DECL_CAST(struct vararg_aggregate_info *, info,
        payload);
    ASMGEN_LABEL(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL,
        sysv_func->func->declaration->identifier,
        appendix_id);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);

    REQUIRE_OK(vararg_load_memory_aggregate(codegen, info));

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RBX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_RBX);
    return KEFIR_OK;
}

struct aggregate_requirements {
    kefir_size_t integers;
    kefir_size_t sse;
};

static kefir_result_t register_aggregate_requirements(struct vararg_aggregate_info *info,
                                                    struct aggregate_requirements *req) {
    req->integers = 0;
    req->sse = 0;
    for (kefir_size_t i = 0; i < kefir_vector_length(&info->arg_alloc->container.qwords); i++) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword,
            kefir_vector_at(&info->arg_alloc->container.qwords, i));
        switch (qword->klass) {
            case KEFIR_AMD64_SYSV_PARAM_INTEGER:
                req->integers++;
                break;

            case KEFIR_AMD64_SYSV_PARAM_SSE:
                req->sse++;
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-integer,sse vararg aggregate members are not supported");
        }
    }
    return KEFIR_OK;
}

static kefir_result_t vararg_get_register_aggregate_intro(struct kefir_codegen_amd64 *codegen,
                                                        const struct kefir_amd64_sysv_function *sysv_func,
                                                        kefir_size_t appendix_id,
                                                        struct vararg_aggregate_info *info,
                                                        struct aggregate_requirements *requirements) {
    REQUIRE_OK(register_aggregate_requirements(info, requirements));
    
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT,
        KEFIR_AMD64_SYSV_ABI_DATA_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        requirements->integers * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CMP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        KEFIR_AMD64_SYSV_INTEGER_REGISTER_COUNT * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JA);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL "_stack",
        sysv_func->func->declaration->identifier,
        appendix_id);
    
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        requirements->sse * 2 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CMP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        KEFIR_AMD64_SYSV_INTEGER_REGISTER_COUNT * KEFIR_AMD64_SYSV_ABI_QWORD +
        2 * KEFIR_AMD64_SYSV_SSE_REGISTER_COUNT * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JA);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL "_stack",
        sysv_func->func->declaration->identifier,
        appendix_id);
    return KEFIR_OK;
}
static kefir_result_t vararg_get_register_aggregate_load(struct kefir_codegen_amd64 *codegen,
                                                        const struct kefir_amd64_sysv_function *sysv_func,
                                                        kefir_size_t appendix_id,
                                                        struct vararg_aggregate_info *info,
                                                        const struct aggregate_requirements *requirements) {
    UNUSED(info);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
        KEFIR_AMD64_SYSV_INTERNAL_BOUND);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSI);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        3 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT,
        KEFIR_AMD64_SYSV_ABI_DATA_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDX);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDX);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSI);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        requirements->integers * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT,
        KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RCX);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RCX);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSI);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        requirements->sse * 2 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    

    kefir_size_t integer_offset = 0, sse_offset = 0;
    for (kefir_size_t i = 0; i < kefir_vector_length(&info->arg_alloc->container.qwords); i++) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword,
            kefir_vector_at(&info->arg_alloc->container.qwords, i));
        switch (qword->klass) {
            case KEFIR_AMD64_SYSV_PARAM_INTEGER:
                ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
                ASMGEN_ARG(&codegen->asmgen,
                    KEFIR_AMD64_INDIRECT_OFFSET,
                    KEFIR_AMD64_RDX,
                    integer_offset * KEFIR_AMD64_SYSV_ABI_QWORD);

                ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG(&codegen->asmgen,
                    KEFIR_AMD64_INDIRECT_OFFSET,
                    KEFIR_AMD64_SYSV_ABI_DATA2_REG,
                    i * KEFIR_AMD64_SYSV_ABI_QWORD);
                ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);

                integer_offset++;
                break;

            case KEFIR_AMD64_SYSV_PARAM_SSE:
                ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
                ASMGEN_ARG(&codegen->asmgen,
                    KEFIR_AMD64_INDIRECT_OFFSET,
                    KEFIR_AMD64_RCX,
                    sse_offset * 2 * KEFIR_AMD64_SYSV_ABI_QWORD);

                ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG(&codegen->asmgen,
                    KEFIR_AMD64_INDIRECT_OFFSET,
                    KEFIR_AMD64_SYSV_ABI_DATA2_REG,
                    i * KEFIR_AMD64_SYSV_ABI_QWORD);
                ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);

                sse_offset++;
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-integer,sse vararg aggregate members are not supported");
        }
    }

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL "_end",
        sysv_func->func->declaration->identifier,
        appendix_id);
    return KEFIR_OK;
}

static kefir_result_t vararg_get_register_aggregate(struct kefir_codegen_amd64 *codegen,
                                 struct kefir_codegen_amd64_sysv_module *sysv_module,
                                 const struct kefir_amd64_sysv_function *sysv_func,
                                 kefir_size_t appendix_id,
                                 void *payload) {
    UNUSED(sysv_module);
    ASSIGN_DECL_CAST(struct vararg_aggregate_info *, info,
        payload);
    struct aggregate_requirements requirements;

    ASMGEN_LABEL(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL,
        sysv_func->func->declaration->identifier,
        appendix_id);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    
    REQUIRE_OK(vararg_get_register_aggregate_intro(codegen, sysv_func, appendix_id, info, &requirements));
    REQUIRE_OK(vararg_get_register_aggregate_load(codegen, sysv_func, appendix_id, info, &requirements));
    ASMGEN_LABEL(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL "_stack",
        sysv_func->func->declaration->identifier,
        appendix_id);
    REQUIRE_OK(vararg_load_memory_aggregate(codegen, info));
    ASMGEN_LABEL(&codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL "_end",
        sysv_func->func->declaration->identifier,
        appendix_id);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RBX);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_INT64_FMT,
        2 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_RBX);
    return KEFIR_OK;
    
    return KEFIR_OK;
}

static kefir_result_t free_vararg_aggregate_info(struct kefir_mem *mem, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(payload != NULL, KEFIR_OK);
    ASSIGN_DECL_CAST(struct vararg_aggregate_info *, info,
        payload);
    REQUIRE_OK(kefir_amd64_sysv_parameter_free(mem, &info->allocation));
    REQUIRE_OK(kefir_vector_free(mem, &info->layout));
    KEFIR_FREE(mem, payload);
    return KEFIR_OK;
}

static kefir_result_t vararg_visit_memory_aggregate(struct vararg_aggregate_info *info,
                                                  struct vararg_getter *param) {
    kefir_size_t appendix_id = param->sysv_func->appendix_index++;
    kefir_result_t res = kefir_amd64_sysv_function_insert_appendix(param->codegen->mem, param->sysv_func,
        vararg_get_memory_aggregate, free_vararg_aggregate_info, info, appendix_id);
    REQUIRE(res == KEFIR_OK || res == KEFIR_ALREADY_EXISTS, res);
    ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
    ASMGEN_ARG(&param->codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL,
        param->sysv_func->func->declaration->identifier,
        appendix_id);
    ASMGEN_ARG0(&param->codegen->asmgen, "0");
    return KEFIR_OK;
}

static kefir_result_t vararg_visit_register_aggregate(struct vararg_aggregate_info *info,
                                                  struct vararg_getter *param) {
    kefir_size_t appendix_id = param->sysv_func->appendix_index++;
    kefir_result_t res = kefir_amd64_sysv_function_insert_appendix(param->codegen->mem, param->sysv_func,
        vararg_get_register_aggregate, free_vararg_aggregate_info, info, appendix_id);
    REQUIRE(res == KEFIR_OK || res == KEFIR_ALREADY_EXISTS, res);
    ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
    ASMGEN_ARG(&param->codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL,
        param->sysv_func->func->declaration->identifier,
        appendix_id);
    ASMGEN_ARG0(&param->codegen->asmgen, "0");
    return KEFIR_OK;
}

static kefir_result_t vararg_visit_aggregate(const struct kefir_ir_type *type,
                                          kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry,
                                          void *payload) {
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct vararg_getter *, param,
        payload);
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));

    struct vararg_aggregate_info *info =
        KEFIR_MALLOC(param->codegen->mem, sizeof(struct vararg_aggregate_info));
    REQUIRE(info != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate memory aggregate vararg info"));

    REQUIRE_OK(kefir_amd64_sysv_type_layout(type, param->codegen->mem, &info->layout));
    kefir_result_t res = kefir_amd64_sysv_parameter_classify(param->codegen->mem, type, &info->layout, &info->allocation);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(param->codegen->mem, &info->layout);
        KEFIR_FREE(param->codegen->mem, info);
        return res;
    });

    info->arg_layout = (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(&info->layout, index);
    info->arg_alloc = (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(&info->allocation, iter.slot);
    REQUIRE_ELSE(info->arg_layout != NULL && info->arg_alloc != NULL, {
        kefir_amd64_sysv_parameter_free(param->codegen->mem, &info->allocation);
        KEFIR_FREE(param->codegen->mem, &info->layout);
        KEFIR_FREE(param->codegen->mem, info);
        return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected to have layout and allocation info for slot");
    });
    if (info->arg_alloc->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY) {
        return vararg_visit_memory_aggregate(info, param);
    } else {
        return vararg_visit_register_aggregate(info, param);
    }
}

kefir_result_t kefir_codegen_amd64_sysv_vararg_instruction(struct kefir_codegen_amd64 *codegen,
                                                struct kefir_codegen_amd64_sysv_module *sysv_module,
                                                struct kefir_amd64_sysv_function *sysv_func,
                                                const struct kefir_irinstr *instr) {
    UNUSED(codegen);
    UNUSED(sysv_module);
    UNUSED(sysv_func);
    switch (instr->opcode) {
        case KEFIR_IROPCODE_VARARG_START: {
            kefir_result_t res = kefir_amd64_sysv_function_insert_appendix(codegen->mem, sysv_func,
                vararg_start, NULL, NULL, KEFIR_AMD64_SYSV_APPENDIX_VARARG_START);
            REQUIRE(res == KEFIR_OK || res == KEFIR_ALREADY_EXISTS, res);

            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG(&codegen->asmgen,
                KEFIR_AMD64_SYSV_FUNCTION_VARARG_START_LABEL,
                sysv_func->func->declaration->identifier);
            ASMGEN_ARG0(&codegen->asmgen, "0");
        } break;

        case KEFIR_IROPCODE_VARARG_COPY:
            break;

        case KEFIR_IROPCODE_VARARG_GET: {
            const kefir_ir_module_id_t type_id = (kefir_ir_module_id_t) instr->arg_pair[0];
            const kefir_size_t type_index = (kefir_size_t) instr->arg_pair[1];
            struct kefir_ir_type *type = kefir_ir_module_get_named_type(sysv_module->module, type_id);
            REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unknown named IR type"));
            struct kefir_ir_type_visitor visitor;
            REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
            KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, vararg_visit_integer);
            KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, vararg_visit_sse);
            visitor.visit[KEFIR_IR_TYPE_STRUCT] = vararg_visit_aggregate;
            visitor.visit[KEFIR_IR_TYPE_UNION] = vararg_visit_aggregate;
            visitor.visit[KEFIR_IR_TYPE_ARRAY] = vararg_visit_aggregate;
            visitor.visit[KEFIR_IR_TYPE_MEMORY] = vararg_visit_aggregate;
            struct vararg_getter param = {
                .codegen = codegen,
                .sysv_module = sysv_module,
                .sysv_func = sysv_func
            };
            REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, &visitor, (void *) &param, type_index, 1));
        } break;

        case KEFIR_IROPCODE_VARARG_END: {
            kefir_result_t res = kefir_amd64_sysv_function_insert_appendix(codegen->mem, sysv_func,
                vararg_end, NULL, NULL, KEFIR_AMD64_SYSV_APPENDIX_VARARG_END);
            REQUIRE(res == KEFIR_OK || res == KEFIR_ALREADY_EXISTS, res);

            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG(&codegen->asmgen,
                KEFIR_AMD64_SYSV_FUNCTION_VARARG_END_LABEL,
                sysv_func->func->declaration->identifier);
            ASMGEN_ARG0(&codegen->asmgen, "0");
        } break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected built-in opcode");
    }
    return KEFIR_OK;
}