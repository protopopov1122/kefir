#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout, mem);

    kefir_irfunction_t func;
    kefir_irfunction_alloc(mem, "insert00", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "insert01", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);


    kefir_irfunction_alloc(mem, "insert02", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "insert10", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "insert11", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);


    kefir_irfunction_alloc(mem, "insert12", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "insert20", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "insert21", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "insert22", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INSERT, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "xchg00", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_XCHG, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "xchg01", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_XCHG, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "xchg02", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_XCHG, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "xchg10", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_XCHG, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "xchg11", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_XCHG, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "xchg12", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_XCHG, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "xchg20", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_XCHG, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "xchg21", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_XCHG, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "xchg22", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_XCHG, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "drop00", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_DROP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "drop01", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_DROP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "drop10", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_DROP, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "drop11", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_DROP, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "drop20", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_DROP, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    kefir_irfunction_alloc(mem, "drop21", 3, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_DROP, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_POP, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_irfunction_free(mem, &func);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    return EXIT_SUCCESS;
}