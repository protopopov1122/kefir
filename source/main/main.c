#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

static void *kefir_malloc(struct kefir_mem *mem, kefir_size_t sz) {
    UNUSED(mem);
    return malloc(sz);
}

static void *kefir_calloc(struct kefir_mem *mem, kefir_size_t num, kefir_size_t sz) {
    UNUSED(mem);
    return calloc(num, sz);
}

static void *kefir_realloc(struct kefir_mem *mem, void *ptr, kefir_size_t sz) {
    UNUSED(mem);
    return realloc(ptr, sz);
}

static void kefir_free(struct kefir_mem *mem, void *ptr) {
    UNUSED(mem);
    free(ptr);
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    struct kefir_mem mem = {
        .malloc = kefir_malloc,
        .calloc = kefir_calloc,
        .realloc = kefir_realloc,
        .free = kefir_free,
        .data = NULL
    };
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout, &mem);

    kefir_irfunction_t func;
    kefir_irfunction_alloc(&mem, "func1", 3, 3, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_STRUCT, 0, 2);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_ARRAY, 0, 2);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_STRUCT, 0, 2);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_ARRAY, 0, 2);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_JMP, 1);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);

    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    kefir_irfunction_free(&mem, &func);
    return EXIT_SUCCESS;
}