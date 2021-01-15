#include "kefir/codegen/amd64/system-v/instr.h"
#include "kefir/codegen/amd64/labels.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/opcodes.h"
#include "kefir/core/error.h"

static kefir_result_t cg_symbolic_opcode(kefir_iropcode_t opcode, const char **symbolic) {
    REQUIRE(symbolic != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected symbolic != NULL"));
    *symbolic = kefir_amd64_iropcode_handler(opcode);
    return *symbolic != NULL ? KEFIR_OK : KEFIR_MALFORMED_ARG;
}

kefir_result_t kefir_amd64_sysv_instruction(struct kefir_codegen_amd64 *codegen,
                                        const struct kefir_amd64_sysv_function *sysv_func,
                                        struct kefir_codegen_amd64_sysv_module *sysv_module,
                                        const struct kefir_irinstr *instr) {
    switch (instr->opcode) {
        case KEFIR_IROPCODE_JMP:
        case KEFIR_IROPCODE_BRANCH: {
            const char *opcode_symbol = NULL;
            REQUIRE(instr->arg >= 0 && instr->arg < (kefir_int64_t) kefir_irblock_length(&sysv_func->func->body),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Jump offset is out of IR block bounds"));
            REQUIRE_OK(cg_symbolic_opcode(instr->opcode, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen,
                KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL " + " KEFIR_INT64_FMT,
                sysv_func->func->declaration->identifier,
                2 * KEFIR_AMD64_SYSV_ABI_QWORD * instr->arg);
        } break;

        case KEFIR_IROPCODE_RET: {
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG(&codegen->asmgen,
                KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL,
                sysv_func->func->declaration->identifier);
            ASMGEN_ARG0(&codegen->asmgen, "0");
        } break;

        case KEFIR_IROPCODE_INVOKE: {
            const char *function = kefir_ir_module_get_named_symbol(sysv_module->module, (kefir_ir_module_id_t) instr->arg);
            REQUIRE(function != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to invoke unknown function"));
            REQUIRE(kefir_codegen_amd64_sysv_module_function_decl(codegen->mem, sysv_module, function),
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AMD64 System-V IR module function decaration"));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG(&codegen->asmgen,
                KEFIR_AMD64_SYSV_FUNCTION_GATE_LABEL,
                function);
            ASMGEN_ARG0(&codegen->asmgen, "0");
        } break;

        case KEFIR_IROPCODE_OFFSETPTR:
        case KEFIR_IROPCODE_ELEMENTPTR: {
            const kefir_ir_module_id_t type_id = (kefir_ir_module_id_t) instr->arg_pair[0];
            const kefir_size_t type_index = (kefir_size_t) instr->arg_pair[1];
            struct kefir_vector *layout =
                kefir_codegen_amd64_sysv_module_type_layout(codegen->mem, sysv_module, type_id);
            REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unknown named type"));
            ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, entry,
                kefir_vector_at(layout, type_index));
            REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to retrieve type node at index"));
            const char *opcode_symbol = NULL;
            if (entry->relative_offset > 0) {
                REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_OFFSETPTR, &opcode_symbol));
                ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
                ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
                ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, entry->relative_offset);
            }
            if (instr->opcode == KEFIR_IROPCODE_ELEMENTPTR && entry->size > 0) {
                REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_ELEMENTPTR, &opcode_symbol));
                ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
                ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
                ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, entry->size);
            }
        } break;

        case KEFIR_IROPCODE_GETLOCALS: {
            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(instr->opcode, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, sysv_func->frame.base.locals);
        } break;

        case KEFIR_IROPCODE_GETGLOBAL: {
            const kefir_ir_module_id_t named_symbol = (kefir_ir_module_id_t) instr->arg;
            const char *symbol = kefir_ir_module_get_named_symbol(sysv_module->module, named_symbol);
            REQUIRE(symbol != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to find named symbol"));

            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_PUSH, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG0(&codegen->asmgen, symbol);
        } break;

        default: {
            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(instr->opcode, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, instr->arg);
        } break;
    }
    return KEFIR_OK;
}