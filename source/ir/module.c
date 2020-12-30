#include "kefir/ir/module.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ir_module_init(struct kefir_ir_module *module) {
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    module->type_head = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_free(struct kefir_mem *mem,
                                struct kefir_ir_module *module) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    struct kefir_ir_module_type *type_current = module->type_head,
                              *type_next = NULL;
    while (type_current != NULL) {
        type_next = type_current->next;
        kefir_ir_type_free(mem, &type_current->type);
        KEFIR_FREE(mem, type_current);
        type_current = type_next;
    }
    module->type_head = NULL;
    return KEFIR_OK;
}

struct kefir_ir_type *kefir_ir_module_new_type(struct kefir_mem *mem,
                                           struct kefir_ir_module *module,
                                           kefir_size_t size) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    struct kefir_ir_module_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_module_type));
    REQUIRE(type != NULL, NULL);
    kefir_result_t result = kefir_ir_type_alloc(mem, size, &type->type);
    REQUIRE_ELSE(result == KEFIR_OK, {
        KEFIR_FREE(mem, type);
        return NULL;
    });
    type->next = module->type_head;
    module->type_head = type;
    return &type->type;
}