#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_irfunction_t func;
    kefir_codegen_amd64_sysv_init(&codegen, stdout, mem);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irfunction_alloc(mem, "func1", 0, 0, 1024, &func));
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_NOP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_JMP, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_BRANCH, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 1000);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PICK, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_DROP, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IADD, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_ISUB, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IMUL, 3);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IDIV, 4);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IMOD, 5);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INEG, 6);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INOT, 7);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IAND, 8);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IOR, 9);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IXOR, 10);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IRSHIFT, 11);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_ILSHIFT, 12);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_COMPARE, 13);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_BAND, 14);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_BOR, 15);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_BNOT, 16);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_ASBOOL, 17);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_TRUNCATE, 18);
    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func));

    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_irfunction_free(mem, &func));
    return KEFIR_OK;
}