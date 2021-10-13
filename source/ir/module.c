/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "kefir/ir/module.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct string_literal {
    kefir_ir_string_literal_type_t type;
    kefir_bool_t public;
    void *content;
    kefir_size_t length;
};

static kefir_result_t destroy_type(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                   void *data) {
    UNUSED(list);
    UNUSED(data);
    struct kefir_ir_type *type = entry->value;
    REQUIRE_OK(kefir_ir_type_free(mem, type));
    KEFIR_FREE(mem, type);
    return KEFIR_OK;
}

static kefir_result_t destroy_function_decl(struct kefir_mem *mem, struct kefir_hashtree *tree,
                                            kefir_hashtree_key_t key, kefir_hashtree_value_t value, void *data) {
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

static kefir_result_t destroy_function(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key,
                                       kefir_hashtree_value_t value, void *data) {
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

static kefir_result_t destroy_named_data(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key,
                                         kefir_hashtree_value_t value, void *data) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(data);
    ASSIGN_DECL_CAST(struct kefir_ir_data *, entry, value);
    REQUIRE_OK(kefir_ir_data_free(mem, entry));
    KEFIR_FREE(mem, entry);
    return KEFIR_OK;
}

static kefir_result_t destroy_string_literal(struct kefir_mem *mem, struct kefir_hashtree *tree,
                                             kefir_hashtree_key_t key, kefir_hashtree_value_t value, void *data) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(data);
    ASSIGN_DECL_CAST(struct string_literal *, literal, value);
    KEFIR_FREE(mem, literal->content);
    KEFIR_FREE(mem, literal);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_alloc(struct kefir_mem *mem, struct kefir_ir_module *module) {
    UNUSED(mem);
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module pointer"));
    REQUIRE_OK(kefir_symbol_table_init(&module->symbols));
    REQUIRE_OK(kefir_list_init(&module->types));
    REQUIRE_OK(kefir_list_on_remove(&module->types, destroy_type, NULL));
    REQUIRE_OK(kefir_hashtree_init(&module->function_declarations, &kefir_hashtree_uint_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&module->function_declarations, destroy_function_decl, NULL));
    REQUIRE_OK(kefir_list_init(&module->global_symbols));
    REQUIRE_OK(kefir_list_init(&module->external_symbols));
    REQUIRE_OK(kefir_hashtree_init(&module->functions, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&module->functions, destroy_function, NULL));
    REQUIRE_OK(kefir_hashtree_init(&module->named_types, &kefir_hashtree_uint_ops));
    REQUIRE_OK(kefir_hashtree_init(&module->named_data, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&module->named_data, destroy_named_data, NULL));
    REQUIRE_OK(kefir_hashtree_init(&module->string_literals, &kefir_hashtree_uint_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&module->string_literals, destroy_string_literal, NULL));
    module->next_type_id = 0;
    module->next_string_literal_id = 0;
    module->next_function_decl_id = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_free(struct kefir_mem *mem, struct kefir_ir_module *module) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module pointer"));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->string_literals));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->named_data));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->named_types));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->functions));
    REQUIRE_OK(kefir_list_free(mem, &module->external_symbols));
    REQUIRE_OK(kefir_list_free(mem, &module->global_symbols));
    REQUIRE_OK(kefir_hashtree_free(mem, &module->function_declarations));
    REQUIRE_OK(kefir_list_free(mem, &module->types));
    REQUIRE_OK(kefir_symbol_table_free(mem, &module->symbols));
    return KEFIR_OK;
}

const char *kefir_ir_module_symbol(struct kefir_mem *mem, struct kefir_ir_module *module, const char *symbol,
                                   kefir_id_t *id) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    REQUIRE(symbol != NULL, NULL);
    return kefir_symbol_table_insert(mem, &module->symbols, symbol, id);
}

kefir_result_t kefir_ir_module_string_literal(struct kefir_mem *mem, struct kefir_ir_module *module,
                                              kefir_ir_string_literal_type_t type, kefir_bool_t public,
                                              const void *content, kefir_size_t length, kefir_id_t *id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));
    REQUIRE(content != NULL && length > 0, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid literal"));
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid literal id pointer"));

    struct string_literal *literal = KEFIR_MALLOC(mem, sizeof(struct string_literal));
    REQUIRE(literal != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate string literal"));

    literal->type = type;
    literal->public = public;
    kefir_size_t sz = 0;
    switch (literal->type) {
        case KEFIR_IR_STRING_LITERAL_MULTIBYTE:
            sz = length;
            break;

        case KEFIR_IR_STRING_LITERAL_UNICODE16:
            sz = length * 2;
            break;

        case KEFIR_IR_STRING_LITERAL_UNICODE32:
            sz = length * 4;
            break;
    }

    literal->content = KEFIR_MALLOC(mem, sz);
    REQUIRE_ELSE(literal->content != NULL, {
        KEFIR_FREE(mem, literal);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate string literal content");
    });

    memcpy(literal->content, content, sz);
    literal->length = length;

    kefir_result_t res =
        kefir_hashtree_insert(mem, &module->string_literals, (kefir_hashtree_key_t) module->next_string_literal_id,
                              (kefir_hashtree_value_t) literal);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, literal->content);
        KEFIR_FREE(mem, literal);
        return res;
    });

    *id = module->next_string_literal_id++;
    return KEFIR_OK;
}

struct kefir_ir_type *kefir_ir_module_new_type(struct kefir_mem *mem, struct kefir_ir_module *module, kefir_size_t size,
                                               kefir_id_t *identifier) {
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
        *identifier = module->next_type_id++;
        result = kefir_hashtree_insert(mem, &module->named_types, (kefir_hashtree_key_t) *identifier,
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

struct kefir_ir_function_decl *kefir_ir_module_new_function_declaration(struct kefir_mem *mem,
                                                                        struct kefir_ir_module *module,
                                                                        const char *name,
                                                                        struct kefir_ir_type *parameters, bool vararg,
                                                                        struct kefir_ir_type *returns) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);

    const char *symbol = kefir_ir_module_symbol(mem, module, name, NULL);
    kefir_id_t func_decl_id = module->next_function_decl_id;
    struct kefir_ir_function_decl *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_function_decl));
    REQUIRE(decl != NULL, NULL);
    kefir_result_t res = kefir_ir_function_decl_alloc(mem, func_decl_id, symbol, parameters, vararg, returns, decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl);
        return NULL;
    });
    res = kefir_hashtree_insert(mem, &module->function_declarations,
                                (kefir_hashtree_key_t) module->next_function_decl_id, (kefir_hashtree_value_t) decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_function_decl_free(mem, decl);
        KEFIR_FREE(mem, decl);
        return NULL;
    });

    module->next_function_decl_id++;
    return decl;
}

kefir_result_t kefir_ir_module_declare_global(struct kefir_mem *mem, struct kefir_ir_module *module,
                                              const char *original) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module pointer"));
    const char *symbol = kefir_ir_module_symbol(mem, module, original, NULL);
    REQUIRE(symbol != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate a symbol"));
    return kefir_list_insert_after(mem, &module->global_symbols, kefir_list_tail(&module->global_symbols),
                                   (void *) symbol);
}

kefir_result_t kefir_ir_module_declare_external(struct kefir_mem *mem, struct kefir_ir_module *module,
                                                const char *original) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module pointer"));
    const char *symbol = kefir_ir_module_symbol(mem, module, original, NULL);
    REQUIRE(symbol != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate a symbol"));
    return kefir_list_insert_after(mem, &module->external_symbols, kefir_list_tail(&module->external_symbols),
                                   (void *) symbol);
}

struct kefir_ir_function *kefir_ir_module_new_function(struct kefir_mem *mem, struct kefir_ir_module *module,
                                                       struct kefir_ir_function_decl *decl,
                                                       struct kefir_ir_type *locals, kefir_size_t length) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);
    REQUIRE(decl != NULL, NULL);
    REQUIRE(decl->name != NULL && strlen(decl->name) != 0, NULL);

    struct kefir_ir_function *func = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_function));
    REQUIRE(func != NULL, NULL);
    kefir_result_t result = kefir_ir_function_alloc(mem, decl, locals, length, func);
    REQUIRE_ELSE(result == KEFIR_OK, {
        KEFIR_FREE(mem, func);
        return NULL;
    });
    result = kefir_hashtree_insert(mem, &module->functions, (kefir_hashtree_key_t) decl->name,
                                   (kefir_hashtree_value_t) func);
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

const char *kefir_ir_module_globals_iter(const struct kefir_ir_module *module, const struct kefir_list_entry **iter) {
    REQUIRE(module != NULL, NULL);
    REQUIRE(iter != NULL, NULL);
    *iter = kefir_list_head(&module->global_symbols);
    REQUIRE(*iter != NULL, NULL);
    return kefir_list_next(iter);
}

const char *kefir_ir_module_externals_iter(const struct kefir_ir_module *module, const struct kefir_list_entry **iter) {
    REQUIRE(module != NULL, NULL);
    REQUIRE(iter != NULL, NULL);
    *iter = kefir_list_head(&module->external_symbols);
    REQUIRE(*iter != NULL, NULL);
    return kefir_list_next((const struct kefir_list_entry **) iter);
}

const char *kefir_ir_module_symbol_iter_next(const struct kefir_list_entry **iter) {
    return kefir_list_next(iter);
}

kefir_result_t kefir_ir_module_get_string_literal(const struct kefir_ir_module *module, kefir_id_t id,
                                                  kefir_ir_string_literal_type_t *type, kefir_bool_t *public,
                                                  const void **content, kefir_size_t *length) {
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string literal type"));
    REQUIRE(public != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to boolean"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid content pointer"));
    REQUIRE(length != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid length pointer"));

    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&module->string_literals, (kefir_hashtree_key_t) id, &node));
    ASSIGN_DECL_CAST(struct string_literal *, literal, node->value);
    *type = literal->type;
    *public = literal->public;
    *content = literal->content;
    *length = literal->length;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_string_literal_iter(const struct kefir_ir_module *module,
                                                   struct kefir_hashtree_node_iterator *iter, kefir_id_t *id,
                                                   kefir_ir_string_literal_type_t *type, kefir_bool_t *public,
                                                   const void **content, kefir_size_t *length) {
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid hash tree iterator pointer"));
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string literal identifier pointer"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string literal type pointer"));
    REQUIRE(public != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to boolean"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string literal content pointer"));
    REQUIRE(length != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string literal length pointer"));

    const struct kefir_hashtree_node *node = kefir_hashtree_iter(&module->string_literals, iter);
    REQUIRE(node != NULL, KEFIR_ITERATOR_END);
    ASSIGN_DECL_CAST(struct string_literal *, literal, node->value);
    *id = (kefir_id_t) node->key;
    *type = literal->type;
    *public = literal->public;
    *content = literal->content;
    *length = literal->length;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_module_string_literal_next(struct kefir_hashtree_node_iterator *iter, kefir_id_t *id,
                                                   kefir_ir_string_literal_type_t *type, kefir_bool_t *public,
                                                   const void **content, kefir_size_t *length) {
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid hash tree iterator pointer"));
    REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string literal identifier pointer"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string literal type pointer"));
    REQUIRE(public != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to boolean"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string literal content pointer"));
    REQUIRE(length != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string literal length pointer"));

    const struct kefir_hashtree_node *node = kefir_hashtree_next(iter);
    REQUIRE(node != NULL, KEFIR_ITERATOR_END);
    ASSIGN_DECL_CAST(struct string_literal *, literal, node->value);
    *id = (kefir_id_t) node->key;
    *type = literal->type;
    *public = literal->public;
    *content = literal->content;
    *length = literal->length;
    return KEFIR_OK;
}

struct kefir_ir_data *kefir_ir_module_new_named_data(struct kefir_mem *mem, struct kefir_ir_module *module,
                                                     const char *identifier, kefir_ir_data_storage_t storage,
                                                     kefir_id_t type_id) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(module != NULL, NULL);

    const struct kefir_ir_type *type = kefir_ir_module_get_named_type(module, type_id);
    REQUIRE(type != NULL, NULL);

    const char *symbol = kefir_ir_module_symbol(mem, module, identifier, NULL);
    REQUIRE(symbol != NULL, NULL);
    struct kefir_ir_data *data = KEFIR_MALLOC(mem, sizeof(struct kefir_ir_data));
    REQUIRE(data != NULL, NULL);
    kefir_result_t res = kefir_ir_data_alloc(mem, storage, type, type_id, data);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, data);
        return NULL;
    });
    res = kefir_hashtree_insert(mem, &module->named_data, (kefir_hashtree_key_t) symbol, (kefir_hashtree_value_t) data);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_data_free(mem, data);
        KEFIR_FREE(mem, data);
        return NULL;
    });
    return data;
}

struct kefir_ir_data *kefir_ir_module_get_named_data(struct kefir_ir_module *module, const char *identifier) {
    REQUIRE(module != NULL, NULL);
    REQUIRE(identifier != NULL, NULL);

    struct kefir_hashtree_node *node = NULL;
    kefir_result_t res = kefir_hashtree_at(&module->named_data, (kefir_hashtree_key_t) identifier, &node);
    REQUIRE(res == KEFIR_OK, NULL);
    ASSIGN_DECL_CAST(struct kefir_ir_data *, data, node->value);
    return data;
}

const struct kefir_ir_data *kefir_ir_module_named_data_iter(const struct kefir_ir_module *module,
                                                            struct kefir_hashtree_node_iterator *iter,
                                                            const char **identifier) {
    REQUIRE(module != NULL, NULL);
    REQUIRE(iter != NULL, NULL);
    const struct kefir_hashtree_node *node = kefir_hashtree_iter(&module->named_data, iter);
    if (node != NULL) {
        if (identifier != NULL) {
            *identifier = (const char *) node->key;
        }
        return (const struct kefir_ir_data *) node->value;
    } else {
        if (identifier != NULL) {
            *identifier = NULL;
        }
        return NULL;
    }
}
const struct kefir_ir_data *kefir_ir_module_named_data_next(struct kefir_hashtree_node_iterator *iter,
                                                            const char **identifier) {
    REQUIRE(iter != NULL, NULL);
    const struct kefir_hashtree_node *node = kefir_hashtree_next(iter);
    if (node != NULL) {
        if (identifier != NULL) {
            *identifier = (const char *) node->key;
        }
        return (const struct kefir_ir_data *) node->value;
    } else {
        if (identifier != NULL) {
            *identifier = NULL;
        }
        return NULL;
    }
}

const char *kefir_ir_module_get_named_symbol(const struct kefir_ir_module *module, kefir_id_t id) {
    REQUIRE(module != NULL, NULL);
    return kefir_symbol_table_get(&module->symbols, id);
}

const struct kefir_ir_function_decl *kefir_ir_module_get_declaration(const struct kefir_ir_module *module,
                                                                     kefir_id_t id) {
    REQUIRE(module != NULL, NULL);
    struct kefir_hashtree_node *node = NULL;
    REQUIRE(kefir_hashtree_at(&module->function_declarations, (kefir_hashtree_key_t) id, &node) == KEFIR_OK, NULL);
    REQUIRE(node != NULL, NULL);
    return (const struct kefir_ir_function_decl *) node->value;
}

struct kefir_ir_type *kefir_ir_module_get_named_type(const struct kefir_ir_module *module, kefir_id_t id) {
    REQUIRE(module != NULL, NULL);
    struct kefir_hashtree_node *node = NULL;
    REQUIRE_ELSE(kefir_hashtree_at(&module->named_types, (kefir_hashtree_key_t) id, &node) == KEFIR_OK,
                 { return NULL; });
    return (struct kefir_ir_type *) node->value;
}
