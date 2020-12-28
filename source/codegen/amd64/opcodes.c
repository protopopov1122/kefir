#include "kefir/codegen/amd64/opcodes.h"
#include "kefir/core/util.h"

#define LABEL(opcode) "__kefirrt_" opcode "_impl"

static struct {
    kefir_iropcode_t opcode;
    const char *handler;
} OPCODE_HANDLERS[] = {
    { KEFIR_IROPCODE_NOP,       LABEL("nop") },
    { KEFIR_IROPCODE_JMP,       LABEL("jmp") },
    { KEFIR_IROPCODE_BRANCH,    LABEL("branch") },
    { KEFIR_IROPCODE_PUSH,      LABEL("push") },
    { KEFIR_IROPCODE_POP,       LABEL("pop") },
    { KEFIR_IROPCODE_PICK,      LABEL("pick") },
    { KEFIR_IROPCODE_DROP,      LABEL("drop") },
    { KEFIR_IROPCODE_IADD,      LABEL("iadd") },
    { KEFIR_IROPCODE_IADD1,     LABEL("iadd1") },
    { KEFIR_IROPCODE_ISUB,      LABEL("isub") },
    { KEFIR_IROPCODE_IMUL,      LABEL("imul") },
    { KEFIR_IROPCODE_IDIV,      LABEL("idiv") },
    { KEFIR_IROPCODE_IMOD,      LABEL("imod") },
    { KEFIR_IROPCODE_INEG,      LABEL("ineg") },
    { KEFIR_IROPCODE_INOT,      LABEL("inot") },
    { KEFIR_IROPCODE_IAND,      LABEL("iand") },
    { KEFIR_IROPCODE_IOR,       LABEL("ior") },
    { KEFIR_IROPCODE_IXOR,      LABEL("ixor") },
    { KEFIR_IROPCODE_IRSHIFT,   LABEL("ishr") },
    { KEFIR_IROPCODE_ILSHIFT,   LABEL("ishl") },
    { KEFIR_IROPCODE_COMPARE,   LABEL("cmp") },
    { KEFIR_IROPCODE_BAND,      LABEL("band") },
    { KEFIR_IROPCODE_BOR,       LABEL("bor") },
    { KEFIR_IROPCODE_BNOT,      LABEL("bnot") },
    { KEFIR_IROPCODE_ASBOOL,    LABEL("asbool") },
    { KEFIR_IROPCODE_TRUNCATE,  LABEL("trunc") }
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