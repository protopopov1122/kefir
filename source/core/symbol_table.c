#include <string.h>
#include "kefir/core/symbol_table.h"
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

kefir_result_t kefir_symbol_table_init(struct kefir_symbol_table *table) {
    REQUIRE(table != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid symbol table pointer"));
    REQUIRE_OK(kefir_hashtree_init(&table->symbols, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&table->symbols, destroy_string, NULL));
    REQUIRE_OK(kefir_hashtree_init(&table->named_symbols, &kefir_hashtree_uint_ops));
    table->next_id = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_symbol_table_free(struct kefir_mem *mem, struct kefir_symbol_table *table) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(table != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid symbol table pointer"));
    REQUIRE_OK(kefir_hashtree_free(mem, &table->named_symbols));
    REQUIRE_OK(kefir_hashtree_free(mem, &table->symbols));
    return KEFIR_OK;
}

static kefir_result_t register_named_symbol(struct kefir_mem *mem,
                                          struct kefir_symbol_table *table,
                                          const char *symbol,
                                          kefir_id_t *id) {
    if (id != NULL) {
        *id = table->next_id++;
        REQUIRE_OK(kefir_hashtree_insert(
            mem, &table->named_symbols, (kefir_hashtree_key_t) *id, (kefir_hashtree_value_t) symbol));
    }
    return KEFIR_OK;
}

const char *kefir_symbol_table_insert(struct kefir_mem *mem,
                                    struct kefir_symbol_table *table,
                                    const char *symbol,
                                    kefir_id_t *id) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(table != NULL, NULL);
    REQUIRE(symbol != NULL, NULL);
    struct kefir_hashtree_node *node;
    kefir_result_t res = kefir_hashtree_at(&table->symbols, (kefir_hashtree_key_t) symbol, &node);
    if (res == KEFIR_OK) {
        REQUIRE_ELSE(register_named_symbol(mem, table, (const char *) node->key, id) == KEFIR_OK, {
            return NULL;
        });
        return (const char *) node->key;
    } else if (res != KEFIR_NOT_FOUND) {
        return NULL;
    }
    char *symbol_copy = KEFIR_MALLOC(mem, strlen(symbol) + 1);
    REQUIRE(symbol_copy != NULL, NULL);
    strcpy(symbol_copy, symbol);
    res = kefir_hashtree_insert(mem, &table->symbols, (kefir_hashtree_key_t) symbol_copy, (kefir_hashtree_value_t) NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, symbol_copy);
        return NULL;
    });
    REQUIRE_ELSE(register_named_symbol(mem, table, symbol_copy, id) == KEFIR_OK, {
        return NULL;
    });
    return symbol_copy;
}

const char *kefir_symbol_table_get(const struct kefir_symbol_table *table,
                                 kefir_id_t id) {
    REQUIRE(table != NULL, NULL);
    struct kefir_hashtree_node *node = NULL;
    REQUIRE_ELSE(kefir_hashtree_at(&table->named_symbols, (kefir_hashtree_key_t) id, &node) == KEFIR_OK, {
        return NULL;
    });
    return (const char *) node->value;   
}
