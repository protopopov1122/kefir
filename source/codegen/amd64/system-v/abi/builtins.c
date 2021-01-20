#include "kefir/codegen/amd64/system-v/abi/builtins.h"

const struct kefir_codegen_amd64_sysv_builtin_type KEFIR_CODEGEN_AMD64_SYSV_BUILTIN_TYPES[] = {
    /* KEFIR_IR_TYPE_BUILTIN_VARARG */ {
        .storage_layout = {
            .size = 3 * KEFIR_AMD64_SYSV_ABI_QWORD,
            .alignment = KEFIR_AMD64_SYSV_ABI_QWORD
        }
    }
};