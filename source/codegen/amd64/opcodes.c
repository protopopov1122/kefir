#include "kefir/codegen/amd64/opcodes.h"
#include "kefir/core/util.h"

#define LABEL(opcode) "__kefirrt_" opcode "_impl"

static struct {
    kefir_iropcode_t opcode;
    const char *handler;
} OPCODE_HANDLERS[] = {
    { KEFIR_IROPCODE_NOP,           LABEL("nop") },
    { KEFIR_IROPCODE_JMP,           LABEL("jmp") },
    { KEFIR_IROPCODE_BRANCH,        LABEL("branch") },
    { KEFIR_IROPCODE_PUSHI64,       LABEL("push") },
    { KEFIR_IROPCODE_POP,           LABEL("pop") },
    { KEFIR_IROPCODE_PICK,          LABEL("pick") },
    { KEFIR_IROPCODE_PUT,           LABEL("put") },
    { KEFIR_IROPCODE_INSERT,        LABEL("insert") },
    { KEFIR_IROPCODE_XCHG,          LABEL("xchg") },
    { KEFIR_IROPCODE_DROP,          LABEL("drop") },
    { KEFIR_IROPCODE_IADD,          LABEL("iadd") },
    { KEFIR_IROPCODE_IADD1,         LABEL("iadd1") },
    { KEFIR_IROPCODE_ISUB,          LABEL("isub") },
    { KEFIR_IROPCODE_IMUL,          LABEL("imul") },
    { KEFIR_IROPCODE_IDIV,          LABEL("idiv") },
    { KEFIR_IROPCODE_IMOD,          LABEL("imod") },
    { KEFIR_IROPCODE_INEG,          LABEL("ineg") },
    { KEFIR_IROPCODE_INOT,          LABEL("inot") },
    { KEFIR_IROPCODE_IAND,          LABEL("iand") },
    { KEFIR_IROPCODE_IOR,           LABEL("ior") },
    { KEFIR_IROPCODE_IXOR,          LABEL("ixor") },
    { KEFIR_IROPCODE_IRSHIFT,       LABEL("ishr") },
    { KEFIR_IROPCODE_IARSHIFT,      LABEL("isar") },
    { KEFIR_IROPCODE_ILSHIFT,       LABEL("ishl") },
    { KEFIR_IROPCODE_IEQUALS,       LABEL("iequals") },
    { KEFIR_IROPCODE_IGREATER,      LABEL("igreater") },
    { KEFIR_IROPCODE_ILESSER,       LABEL("ilesser") },
    { KEFIR_IROPCODE_IABOVE,        LABEL("iabove") },
    { KEFIR_IROPCODE_IBELOW,        LABEL("ibelow") },
    { KEFIR_IROPCODE_BAND,          LABEL("band") },
    { KEFIR_IROPCODE_BOR,           LABEL("bor") },
    { KEFIR_IROPCODE_BNOT,          LABEL("bnot") },
    { KEFIR_IROPCODE_TRUNCATE1,     LABEL("trunc1") },
    { KEFIR_IROPCODE_EXTEND8,       LABEL("extend8") },
    { KEFIR_IROPCODE_EXTEND16,      LABEL("extend16") },
    { KEFIR_IROPCODE_EXTEND32,      LABEL("extend32") },
    { KEFIR_IROPCODE_OFFSETPTR,     LABEL("offsetptr") },
    { KEFIR_IROPCODE_ELEMENTPTR,    LABEL("elementptr") },
    { KEFIR_IROPCODE_LOAD8U,        LABEL("load8u") },
    { KEFIR_IROPCODE_LOAD8I,        LABEL("load8i") },
    { KEFIR_IROPCODE_LOAD16U,       LABEL("load16u") },
    { KEFIR_IROPCODE_LOAD16I,       LABEL("load16i") },
    { KEFIR_IROPCODE_LOAD32U,       LABEL("load32u") },
    { KEFIR_IROPCODE_LOAD32I,       LABEL("load32i") },
    { KEFIR_IROPCODE_LOAD64,        LABEL("load64") },
    { KEFIR_IROPCODE_STORE8,        LABEL("store8") },
    { KEFIR_IROPCODE_STORE16,       LABEL("store16") },
    { KEFIR_IROPCODE_STORE32,       LABEL("store32") },
    { KEFIR_IROPCODE_STORE64,       LABEL("store64") },
    { KEFIR_IROPCODE_GETLOCALS,     LABEL("getlocals") },
    { KEFIR_IROPCODE_F32ADD,        LABEL("f32add") },
    { KEFIR_IROPCODE_F32SUB,        LABEL("f32sub") },
    { KEFIR_IROPCODE_F32MUL,        LABEL("f32mul") },
    { KEFIR_IROPCODE_F32DIV,        LABEL("f32div") },
    { KEFIR_IROPCODE_F64ADD,        LABEL("f64add") },
    { KEFIR_IROPCODE_F64SUB,        LABEL("f64sub") },
    { KEFIR_IROPCODE_F64MUL,        LABEL("f64mul") },
    { KEFIR_IROPCODE_F64DIV,        LABEL("f64div") },
    { KEFIR_IROPCODE_F32EQUALS,     LABEL("f32equals") },
    { KEFIR_IROPCODE_F32GREATER,    LABEL("f32greater") },
    { KEFIR_IROPCODE_F32LESSER,     LABEL("f32lesser") },
    { KEFIR_IROPCODE_F64EQUALS,     LABEL("f64equals") },
    { KEFIR_IROPCODE_F64GREATER,    LABEL("f64greater") },
    { KEFIR_IROPCODE_F64LESSER,     LABEL("f64lesser") },
    { KEFIR_IROPCODE_F32CINT,       LABEL("f32cint") },
    { KEFIR_IROPCODE_F64CINT,       LABEL("f64cint") },
    { KEFIR_IROPCODE_INTCF32,       LABEL("intcf32") },
    { KEFIR_IROPCODE_INTCF64,       LABEL("intcf64") },
    { KEFIR_IROPCODE_F32CF64,       LABEL("f32cf64") },
    { KEFIR_IROPCODE_F64CF32,       LABEL("f64cf32") }
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