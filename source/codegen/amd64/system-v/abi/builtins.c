#include "kefir/codegen/amd64/system-v/abi/builtins.h"
#include "kefir/codegen/amd64/system-v/abi/qwords.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/labels.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/core/error.h"

const struct kefir_codegen_amd64_sysv_builtin_type KEFIR_CODEGEN_AMD64_SYSV_BUILTIN_TYPES[] = {
    /* KEFIR_IR_TYPE_BUILTIN_VARARG */ {
        .storage_layout = {
            .size = 4 * KEFIR_AMD64_SYSV_ABI_QWORD,
            .alignment = KEFIR_AMD64_SYSV_ABI_QWORD
        }
    }
};

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
    kefir_size_t appendix_id = param->sysv_func->appendix_index++;
    kefir_result_t res = kefir_amd64_sysv_function_insert_appendix(param->codegen->mem, param->sysv_func,
        vararg_get_int, NULL, NULL, appendix_id);
    REQUIRE(res == KEFIR_OK || res == KEFIR_ALREADY_EXISTS, res);
    ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
    ASMGEN_ARG(&param->codegen->asmgen,
        KEFIR_AMD64_SYSV_FUNCTION_VARARG_ARG_LABEL,
        param->sysv_func->func->declaration->identifier,
        appendix_id);
    ASMGEN_ARG0(&param->codegen->asmgen, "0");
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_builtin_instruction(struct kefir_codegen_amd64 *codegen,
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