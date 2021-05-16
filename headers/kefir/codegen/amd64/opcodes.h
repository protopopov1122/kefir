#ifndef KEFIR_CODEGEN_AMD64_OPCODES_H_
#define KEFIR_CODEGEN_AMD64_OPCODES_H_

#include "kefir/core/basic-types.h"
#include "kefir/ir/opcodes.h"

const char *kefir_amd64_iropcode_handler(kefir_iropcode_t);
kefir_result_t kefir_amd64_iropcode_handler_list(kefir_result_t (*)(kefir_iropcode_t, const char *, void *), void *);

#endif
