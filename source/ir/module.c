#include <string.h>
#include "kefir/ir/module.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t destroy_string(struct kefir_mem *mem,
                                   struct kefir_hashtree *tree,
                                   kefir_hashtree_key_t key,
                                   kefir_hashtree_value_t value,
                                   void *data) {
    UNUSED(tree);
    UNUSED(value);
    UNUSED(data);
    KEFIR_FREE(mem, (char *) key);
    return KEFIR_OK;
}

static kefir_result_t destroy_type(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry, void *data) {
    UNUSED(list);
    UNUSED(data);
    struct kefir_ir_type *type = entry->value;
    REQUIRE_OK(kefir_ir_type_free(mem, type));
    KEFIR_FREE(mem, type);
    return KEFIR_OK;
}

static kefir_result_t destroy_function_decl(struct kefir_mem *mem,
                                          struct kefir_hashtree *tree,
                                          kefir_hashtree_key_t key,
                                          kefir_hashtree_value_t value,
                                          void *data) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(data);
    ASSIGN_DECL_CAST(struct kefir_ir_function_decl *, decl, value);
    if (decl != NULL) {
        REQUIRE_OK(kefir_ir_function_decl_free(mem, decl));
        KEFIR_FREE(mem, decl);
    }
    return KEFIR_OK;
}

static kefir_result_t destroy_function(struct kefir_mem *mem,
                                     struct kefir_hashtree *tree,
                                     kefir_hashtree_key_t key,
                                     kefir_hashtree_value_t value,
                                     void *data) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(data);
    ASSIGN_DECL_CAST(struct kefir_ir_function *, func, value);
    if (func != NULL) {
        REQUIRE_OK(kefir_ir_function_free(mem, func));
        KEFIR_FREE(mem, func);
    }
    return KEFIR_OK;
}

static kefir_result_t destroy_string_value(struct kefir_mem *mem,
                                         struct kefir_hashtree *tree,
                                         kefir_hashtree_key_t key,
                                         kefir_hashtree_value_t value,
                                         void *data) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(data);
    KEFIR_FREE(mem, (char *) value);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_alloc(struct kefir_mem *mem, struct kefir_ir_module *module) {
    UNUSED(mem);
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    REQUIRE_OK(kefir_hashtree_init(&module->symbols, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&module->symbols, destroy_string, NULL));
    REQUIRE_OK(kefir_list_init(&module->types));
    REQUIRE_OK(kefir_list_on_remove(&module->types, destroy_type, NULL));
    REQUIRE_OK(kefir_hashtree_init(&module->function_declarations, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&module->function_declarations, destroy_function_decl, NULL));
    REQUIRE_OK(kefir_list_init(&module->global_symbols));
    REQUIRE_OK(kefir_list_init(&module->external_symbols));
    REQUIRE_OK(kefir_hashtree_init(&module->functions, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&module->functions, destroy_function, NULL));
    REQUIRE_OK(kefir_hashtree_init(&module->named_types, &kefir_hashtree_uint_ops));
    REQUIRE_OK(kefir_hashtree_init(&module->named_strings, &kefir_hashtree_uint_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&module->named_strings, destroy_string_value, NULL));
    module->next_type_id = 0;
    module->next_string_id = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_free(struct kefir_mem *mem,
                                struct kefir_ir_module *module) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->named_strings));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->named_types));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->functions));
    REQUIRE_OK(kefir_list_free(mem, &module->external_symbols));
    REQUIRE_OK(kefir_list_free(mem, &module->global_symbols));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->function_declarations));
    REQUIRE_OK(kefir_list_free(mem, &module->types));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->symbols));
    return KEFIR_OK;
}

const char *kefir_ir_module_symbol(struct kefir_mem *mem,
                                 struct kefir_ir_module *module,
                                 const char *symbol) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    REQUIRE(symbol != NULL, NULL);
    struct kefir_hashtree_node *node;
    kefir_result_t res = kefir_hashtree_at(&module->symbols, (kefir_hashtree_key_t) symbol, &node);
    if (res == KEFIR_OK) {
        return (const char *) node->key;
    } else if (res != KEFIR_NOT_FOUND) {
        return NULL;
    }
    char *symbol_copy = KEFIR_MALLOC(mem, strlen(symbol) + 1);
    REQUIRE(symbol_copy != NULL, NULL);
    strcpy(symbol_copy, symbol);
    res = kefir_hashtree_insert(mem, &module->symbols, (kefir_hashtree_key_t) symbol_copy, (kefir_hashtree_value_t) NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, symbol_copy);
        return NULL;
    });
    return symbol_copy;
}

struct kefir_ir_type *kefir_ir_module_new_type(struct kefir_mem *mem,
                                           struct kefir_ir_module *module,
                                           kefir_size_t size,
                                           kefir_ir_module_id_t *identifier) {
    UNUSED(identifier);
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    struct kefir_ir_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_type));
    kefir_result_t result = kefir_ir_type_alloc(mem, size, type);
    REQUIRE_ELSE(result == KEFIR_OK, {
        KEFIR_FREE(mem, type);
        return NULL;
    });
    result = kefir_list_insert_after(mem, &module->types, kefir_list_tail(&module->types), type);
    REQUIRE_ELSE(result == KEFIR_OK, {
        kefir_ir_type_free(mem, type);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    if (identifier != NULL) {
        result = kefir_hashtree_insert(mem,
                                     &module->named_types,
                                     (kefir_hashtree_key_t) module->next_type_id++,
                                     (kefir_hashtree_value_t) type);
        REQUIRE_ELSE(result == KEFIR_OK, {
            kefir_list_pop(mem, &module->types, kefir_list_tail(&module->types));
            kefir_ir_type_free(mem, type);
            KEFIR_FREE(mem, type);
            return NULL;
        });     
    }
    return type;
}

kefir_result_t kefir_ir_module_string(struct kefir_mem *mem,
                                  struct kefir_ir_module *module,
                                  const char *string,
                                  kefir_ir_module_id_t *id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    REQUIRE(string != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string"));
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid module ID pointer"));
    char *copy = KEFIR_MALLOC(mem, strlen(string) + 1);
    REQUIRE(copy != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate a string"));
    strcpy(copy, string);
    kefir_ir_module_id_t new_id = module->next_string_id++;
    kefir_result_t res = kefir_hashtree_insert(mem, &module->symbols, (kefir_hashtree_key_t) new_id, (kefir_hashtree_value_t) copy);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, copy);
        return res;
    });
    *id = new_id;
    return KEFIR_OK;
}

struct kefir_ir_function_decl *kefir_ir_module_new_function_declaration(struct kefir_mem *mem,
                                                                    struct kefir_ir_module *module,
                                                                    const char *identifier,
                                                                    struct kefir_ir_type *parameters,
                                                                    struct kefir_ir_type *returns) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    const char *symbol = kefir_ir_module_symbol(mem, module, identifier);
    REQUIRE(symbol != NULL, NULL);
    struct kefir_ir_function_decl *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_function_decl));
    REQUIRE(decl != NULL, NULL);
    kefir_result_t res = kefir_ir_function_decl_alloc(mem, symbol, parameters, returns, decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl);
        return NULL;
    });
    res = kefir_hashtree_insert(mem, &module->function_declarations, (kefir_hashtree_key_t) decl->identifier, (kefir_hashtree_value_t) decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_function_decl_free(mem, decl);
        KEFIR_FREE(mem, decl);
        return NULL;
    });
    return decl;
}

kefir_result_t kefir_ir_module_declare_global(struct kefir_mem *mem,
                                          struct kefir_ir_module *module,
                                          const char *original) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    const char *symbol = kefir_ir_module_symbol(mem, module, original);
    REQUIRE(symbol != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate a symbol"));
    return kefir_list_insert_after(mem, &module->global_symbols, kefir_list_tail(&module->global_symbols), (void *) symbol);
}

kefir_result_t kefir_ir_module_declare_external(struct kefir_mem *mem,
                                          struct kefir_ir_module *module,
                                          const char *original) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module pointer"));
    const char *symbol = kefir_ir_module_symbol(mem, module, original);
    REQUIRE(symbol != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate a symbol"));
    return kefir_list_insert_after(mem, &module->external_symbols, kefir_list_tail(&module->external_symbols), (void *) symbol);
}

struct kefir_ir_function * kefir_ir_module_new_function(struct kefir_mem *mem,
                                                    struct kefir_ir_module *module,
                                                    const char *identifier,
                                                    kefir_size_t length) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    struct kefir_hashtree_node *decl_node;
    REQUIRE(kefir_hashtree_at(&module->function_declarations, (kefir_hashtree_key_t) identifier, &decl_node) == KEFIR_OK, NULL);
    struct kefir_ir_function_decl *decl = (struct kefir_ir_function_decl *) decl_node->value;
    REQUIRE(decl != NULL, NULL);
    struct kefir_ir_function *func = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_function));
    REQUIRE(func != NULL, NULL);
    kefir_result_t result = kefir_ir_function_alloc(mem, decl, length, func);
    REQUIRE_ELSE(result == KEFIR_OK, {
        KEFIR_FREE(mem, func);
        return NULL;
    });
    result = kefir_hashtree_insert(mem, &module->functions, (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) func);
    REQUIRE_ELSE(result == KEFIR_OK, {
        kefir_ir_function_free(mem, func);
        KEFIR_FREE(mem, func);
        return NULL;
    });
    return func;
}

const struct kefir_ir_function *kefir_ir_module_function_iter(const struct kefir_ir_module *module,
                                                          struct kefir_hashtree_node_iterator *iter) {
    REQUIRE(module != NULL, NULL); 
    REQUIRE(iter != NULL, NULL);
    const struct kefir_hashtree_node *node = kefir_hashtree_iter(&module->functions, iter);
    if (node != NULL) {
        return (const struct kefir_ir_function *) node->value;
    } else {
        return NULL;
    }
}
const struct kefir_ir_function *kefir_ir_module_function_next(struct kefir_hashtree_node_iterator *iter) {
    REQUIRE(iter != NULL, NULL);
    const struct kefir_hashtree_node *node = kefir_hashtree_next(iter);
    if (node != NULL) {
        return (const struct kefir_ir_function *) node->value;
    } else {
        return NULL;
    }
}