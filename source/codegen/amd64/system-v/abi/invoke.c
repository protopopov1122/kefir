#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/runtime.h"

struct invoke_info {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_amd64_sysv_function_decl *decl;
    const kefir_size_t total_arguments;
    kefir_size_t argument;
};

struct invoke_returns {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_amd64_sysv_function_decl *decl;
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

static kefir_result_t scalar_argument(const struct kefir_ir_type *type,
                                     kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry,
                                     void *payload) {
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct invoke_info *, info,
        payload);
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, allocation,
        kefir_vector_at(&info->decl->parameters.allocation, iter.slot));
    switch (allocation->klass) {
        case KEFIR_AMD64_SYSV_PARAM_INTEGER:
            ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&info->codegen->asmgen,
                KEFIR_AMD64_SYSV_INTEGER_REGISTERS[allocation->location.integer_register]);
            ASMGEN_ARG(&info->codegen->asmgen,
                KEFIR_AMD64_INDIRECT_OFFSET,
                KEFIR_AMD64_SYSV_ABI_DATA_REG,
                (info->total_arguments - info->argument - 1) * KEFIR_AMD64_SYSV_ABI_QWORD);
            break;

        case KEFIR_AMD64_SYSV_PARAM_SSE:
            ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PINSRQ);
            ASMGEN_ARG0(&info->codegen->asmgen,
                KEFIR_AMD64_SYSV_SSE_REGISTERS[allocation->location.sse_register]);
            ASMGEN_ARG(&info->codegen->asmgen,
                KEFIR_AMD64_INDIRECT_OFFSET,
                KEFIR_AMD64_SYSV_ABI_DATA_REG,
                (info->total_arguments - info->argument - 1) * KEFIR_AMD64_SYSV_ABI_QWORD);
            ASMGEN_ARG0(&info->codegen->asmgen, "0");
            break;

        case KEFIR_AMD64_SYSV_PARAM_MEMORY:
            ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&info->codegen->asmgen,
                KEFIR_AMD64_SYSV_ABI_TMP_REG);
            ASMGEN_ARG(&info->codegen->asmgen,
                KEFIR_AMD64_INDIRECT_OFFSET,
                KEFIR_AMD64_SYSV_ABI_DATA_REG,
                (info->total_arguments - info->argument - 1) * KEFIR_AMD64_SYSV_ABI_QWORD);
            ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG(&info->codegen->asmgen,
                KEFIR_AMD64_INDIRECT_OFFSET,
                KEFIR_AMD64_RSP,
                allocation->location.stack_offset);
            ASMGEN_ARG0(&info->codegen->asmgen,
                KEFIR_AMD64_SYSV_ABI_TMP_REG);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Integer function argument cannot have non-INTEGER and non-MEMORY class");
    }
    info->argument++;
    return KEFIR_OK;
}

kefir_result_t invoke_prologue(struct kefir_codegen_amd64 *codegen,
                                            const struct kefir_amd64_sysv_function_decl *decl,
                                            struct invoke_info *info) {
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, scalar_argument);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, scalar_argument);
    
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
    if (decl->parameters.location.stack_offset > 0) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, decl->parameters.location.stack_offset);
    }
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_AND);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, ~0xfll);
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(
        decl->decl->params, &visitor, (void *) info, 0, info->total_arguments));
    return KEFIR_OK;
}

static kefir_result_t integer_return(const struct kefir_ir_type *type,
                                     kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry,
                                     void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct invoke_info *, info,
        payload);
    ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RAX);
    return KEFIR_OK;
}

static kefir_result_t sse_return(const struct kefir_ir_type *type,
                                     kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry,
                                     void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct invoke_info *, info,
        payload);
    ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PEXTRQ);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_XMM0);
    ASMGEN_ARG0(&info->codegen->asmgen, "0");
    ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
    return KEFIR_OK;
}

kefir_result_t invoke_epilogue(struct kefir_codegen_amd64 *codegen,
                                            const struct kefir_amd64_sysv_function_decl *decl,
                                            struct invoke_info *info) {
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    if (info->total_arguments > 0) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, info->total_arguments * KEFIR_AMD64_SYSV_ABI_QWORD);
    }
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, integer_return);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, sse_return);
    REQUIRE(kefir_ir_type_nodes(decl->decl->result) <= 1, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function cannot return more than one value"));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(decl->decl->result, &visitor, (void *) info, 0, 1));
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_invoke(struct kefir_codegen_amd64 *codegen,
                                            const struct kefir_amd64_sysv_function_decl *decl) {                                          
    struct invoke_info info = {
        .codegen = codegen,
        .decl = decl,
        .total_arguments = kefir_ir_type_nodes(decl->decl->params),
        .argument = 0
    };
    REQUIRE_OK(invoke_prologue(codegen, decl, &info));
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CALL);
    ASMGEN_ARG0(&codegen->asmgen, decl->decl->identifier);
    REQUIRE_OK(invoke_epilogue(codegen, decl, &info));
    return KEFIR_OK;
}