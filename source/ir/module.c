#include "kefir/ir/module.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t destroy_function_decl(struct kefir_mem *mem,
                                          struct kefir_hashtree *tree,
                                          const char *key,
                                          void *value,
                                          void *data) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(data);
    ASSIGN_DECL_CAST(struct kefir_ir_function_decl *, decl, value);
    if (value != NULL) {
        REQUIRE_OK(kefir_ir_function_decl_free(mem, decl));
        KEFIR_FREE(mem, decl);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_alloc(struct kefir_mem *mem, struct kefir_ir_module *module) {
    UNUSED(mem);
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    REQUIRE_OK(kefir_list_init(&module->types, sizeof(struct kefir_ir_type)));
    REQUIRE_OK(kefir_hashtree_init(&module->function_declarations));
    REQUIRE_OK(kefir_hashtree_on_removal(&module->function_declarations, destroy_function_decl, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_free(struct kefir_mem *mem,
                                struct kefir_ir_module *module) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->function_declarations));
    void *iter = kefir_list_iter(&module->types);
    for (struct kefir_ir_type *type = kefir_list_next(&iter); type != NULL; type = kefir_list_next(&iter)) {
        kefir_ir_type_free(mem, type);
    }
    kefir_list_free(mem, &module->types);
    return KEFIR_OK;
}

struct kefir_ir_type *kefir_ir_module_new_type(struct kefir_mem *mem,
                                           struct kefir_ir_module *module,
                                           kefir_size_t size) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    struct kefir_ir_type *type = kefir_list_append(mem, &module->types, NULL);
    REQUIRE(type != NULL, NULL);
    kefir_result_t result = kefir_ir_type_alloc(mem, size, type);
    REQUIRE_ELSE(result == KEFIR_OK, {
        kefir_list_pop_back(mem, &module->types);
        return NULL;
    });
    return type;
}

struct kefir_ir_function_decl *kefir_ir_module_new_function_declaration(struct kefir_mem *mem,
                                                                    struct kefir_ir_module *module,
                                                                    const char *identifier,
                                                                    struct kefir_ir_type *parameters,
                                                                    struct kefir_ir_type *returns) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    struct kefir_ir_function_decl *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_function_decl));
    REQUIRE(decl != NULL, NULL);
    kefir_result_t res = kefir_ir_function_decl_alloc(mem, identifier, parameters, returns, decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl);
        return NULL;
    });
    res = kefir_hashtree_insert(mem, &module->function_declarations, decl->identifier, (void *) decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_function_decl_free(mem, decl);
        KEFIR_FREE(mem, decl);
        return NULL;
    });
    return decl;
}