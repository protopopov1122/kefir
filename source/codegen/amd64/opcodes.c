#include "kefir/codegen/amd64/opcodes.h"
#include "kefir/core/util.h"

static struct {
    kefir_iropcode_t opcode;
    const char *handler;
} OPCODE_HANDLERS[] = {
    { KEFIR_IROPCODE_NOP, "__kefirrt_nop_impl" },
    { KEFIR_IROPCODE_PUSH, "__kefirrt_push_impl" },
    { KEFIR_IROPCODE_POP, "__kefirrt_pop_impl" },
    { KEFIR_IROPCODE_IADD, "__kefirrt_iadd_impl" },
    { KEFIR_IROPCODE_RET, "__kefirrt_ret_impl" }
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