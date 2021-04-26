#ifndef CODEGEN_H_
#define CODEGEN_H_

#define GENCODE(fn) \
    do { \
        struct kefir_codegen_amd64 codegen; \
        struct kefir_ir_target_platform ir_target; \
        REQUIRE_OK(kefir_codegen_amd64_sysv_target_platform(&ir_target)); \
        REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout)); \
        struct kefir_ir_module module; \
        REQUIRE_OK(kefir_ir_module_alloc(mem, &module)); \
        REQUIRE_OK((fn)(mem, &module, &ir_target)); \
        KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module); \
        REQUIRE_OK(kefir_ir_module_free(mem, &module)); \
        KEFIR_CODEGEN_CLOSE(&codegen.iface); \
    } while (0)

struct function {
    const struct kefir_ast_type *type;
    struct kefir_ast_local_context local_context;
    struct kefir_list args;
    struct kefir_ast_node_base *body;
};

#endif