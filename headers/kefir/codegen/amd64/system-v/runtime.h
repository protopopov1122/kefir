#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_RUNTIME_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_RUNTIME_H_

#include "kefir/core/basic-types.h"

#define KEFIR_AMD64_SYSTEM_V_RUNTIME_PRESERVE_STATE "__kefirrt_preserve_state"
#define KEFIR_AMD64_SYSTEM_V_RUNTIME_RESTORE_STATE "__kefirrt_restore_state"

extern const char *KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOLS[];
extern kefir_size_t KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOL_COUNT;

#endif