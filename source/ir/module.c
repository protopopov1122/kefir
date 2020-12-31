#include "kefir/ir/module.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ir_module_init(struct kefir_ir_module *module) {
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    kefir_list_init(&module->type_head, sizeof(struct kefir_ir_type));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_free(struct kefir_mem *mem,
                                struct kefir_ir_module *module) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    void *iter = kefir_list_iter(&module->type_head);
    for (struct kefir_ir_type *type = kefir_list_next(&iter); type != NULL; type = kefir_list_next(&iter)) {
        kefir_ir_type_free(mem, type);
    }
    kefir_list_free(mem, &module->type_head);
    return KEFIR_OK;
}

struct kefir_ir_type *kefir_ir_module_new_type(struct kefir_mem *mem,
                                           struct kefir_ir_module *module,
                                           kefir_size_t size) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    struct kefir_ir_type *type = kefir_list_append(mem, &module->type_head, NULL);
    REQUIRE(type != NULL, NULL);
    kefir_result_t result = kefir_ir_type_alloc(mem, size, type);
    REQUIRE_ELSE(result == KEFIR_OK, {
        kefir_list_pop_back(mem, &module->type_head);
        return NULL;
    });
    return type;
}