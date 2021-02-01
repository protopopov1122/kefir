#include "kefir/codegen/amd64/opcodes.h"
#include "kefir/core/util.h"
#include "kefir/ir/mnemonic.h"

#define LABEL(opcode) "__kefirrt_" opcode "_impl"
#define HANDLER(opcode) { KEFIR_IROPCODE_##opcode, LABEL(KEFIR_IROPCODE_MNEMONIC_##opcode) }

static struct {
    kefir_iropcode_t opcode;
    const char *handler;
} OPCODE_HANDLERS[] = {
    HANDLER(NOP),
    HANDLER(JMP),
    HANDLER(BRANCH),
    HANDLER(PUSHI64),
    HANDLER(POP),
    HANDLER(PICK),
    HANDLER(PUT),
    HANDLER(INSERT),
    HANDLER(XCHG),
    HANDLER(DROP),
    HANDLER(IADD),
    HANDLER(IADD1),
    HANDLER(ISUB),
    HANDLER(IMUL),
    HANDLER(IDIV),
    HANDLER(IMOD),
    HANDLER(INEG),
    HANDLER(INOT),
    HANDLER(IAND),
    HANDLER(IOR),
    HANDLER(IXOR),
    HANDLER(IRSHIFT),
    HANDLER(IARSHIFT),
    HANDLER(ILSHIFT),
    HANDLER(IEQUALS),
    HANDLER(IGREATER),
    HANDLER(ILESSER),
    HANDLER(IABOVE),
    HANDLER(IBELOW),
    HANDLER(BAND),
    HANDLER(BOR),
    HANDLER(BNOT),
    HANDLER(TRUNCATE1),
    HANDLER(EXTEND8),
    HANDLER(EXTEND16),
    HANDLER(EXTEND32),
    HANDLER(OFFSETPTR),
    HANDLER(ELEMENTPTR),
    HANDLER(LOAD8U),
    HANDLER(LOAD8I),
    HANDLER(LOAD16U),
    HANDLER(LOAD16I),
    HANDLER(LOAD32U),
    HANDLER(LOAD32I),
    HANDLER(LOAD64),
    HANDLER(STORE8),
    HANDLER(STORE16),
    HANDLER(STORE32),
    HANDLER(STORE64),
    HANDLER(GETLOCALS),
    HANDLER(F32ADD),
    HANDLER(F32SUB),
    HANDLER(F32MUL),
    HANDLER(F32DIV),
    HANDLER(F64ADD),
    HANDLER(F64SUB),
    HANDLER(F64MUL),
    HANDLER(F64DIV),
    HANDLER(F32EQUALS),
    HANDLER(F32GREATER),
    HANDLER(F32LESSER),
    HANDLER(F64EQUALS),
    HANDLER(F64GREATER),
    HANDLER(F64LESSER),
    HANDLER(F32CINT),
    HANDLER(F64CINT),
    HANDLER(INTCF32),
    HANDLER(INTCF64),
    HANDLER(F32CF64),
    HANDLER(F64CF32)
};

const char *kefir_amd64_iropcode_handler(kefir_iropcode_t opcode) {
    for (kefir_size_t i = 0; i < sizeof(OPCODE_HANDLERS) / sizeof(OPCODE_HANDLERS[0]); i++) {
        if (OPCODE_HANDLERS[i].opcode == opcode) {
            return OPCODE_HANDLERS[i].handler;
        }
    }
    return NULL;
}

kefir_result_t kefir_amd64_iropcode_handler_list(kefir_result_t (*callback)(kefir_iropcode_t, const char *, void *), void *payload) {
    for (kefir_size_t i = 0; i < sizeof(OPCODE_HANDLERS) / sizeof(OPCODE_HANDLERS[0]); i++) {
        REQUIRE_OK(callback(OPCODE_HANDLERS[i].opcode, OPCODE_HANDLERS[i].handler, payload));
    }
    return KEFIR_OK;
}