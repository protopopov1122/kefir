#ifndef KEFIR_CORE_SYMBOL_TABLE_H_
#define KEFIR_CORE_SYMBOL_TABLE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/hashtree.h"

typedef struct kefir_symbol_table {
    struct kefir_hashtree symbols;
    struct kefir_hashtree symbol_identifiers;
    kefir_size_t next_id;
} kefir_symbol_table_t;

kefir_result_t kefir_symbol_table_init(struct kefir_symbol_table *);
kefir_result_t kefir_symbol_table_free(struct kefir_mem *, struct kefir_symbol_table *);
const char *kefir_symbol_table_insert(struct kefir_mem *,
                                    struct kefir_symbol_table *,
                                    const char *,
                                    kefir_id_t *);
const char *kefir_symbol_table_get(const struct kefir_symbol_table *,
                                 kefir_id_t);

#endif
