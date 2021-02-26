#ifndef KEFIR_AST_TYPE_ENUM_H_
#define KEFIR_AST_TYPE_ENUM_H_

#include "kefir/ast/base.h"
#include "kefir/ast/type/base.h"

typedef struct kefir_ast_enum_enumerator {
    const char *identifier;
    struct kefir_ast_constant_expression *value;
} kefir_ast_enum_enumerator_t;

typedef struct kefir_ast_enum_type {
    kefir_bool_t complete;
    const char *identifier;
    struct kefir_list enumerators;
    struct kefir_hashtree enumerator_index;
} kefir_ast_enum_type_t;

kefir_result_t kefir_ast_enumeration_get(const struct kefir_ast_enum_type *,
                                     const char *,
                                     const struct kefir_ast_constant_expression **);

const struct kefir_ast_type *kefir_ast_type_incomplete_enumeration(struct kefir_mem *,
                                                               struct kefir_ast_type_storage *,
                                                               const char *);

kefir_result_t kefir_ast_enumeration_type_constant(struct kefir_mem *,
                                               struct kefir_symbol_table *,
                                               struct kefir_ast_enum_type *,
                                               const char *,
                                               struct kefir_ast_constant_expression *);

kefir_result_t kefir_ast_enumeration_type_constant_auto(struct kefir_mem *,
                                                    struct kefir_symbol_table *,
                                                    struct kefir_ast_enum_type *,
                                                    const char *);

const struct kefir_ast_type *kefir_ast_enumeration_underlying_type(const struct kefir_ast_enum_type *);

const struct kefir_ast_type *kefir_ast_type_enumeration(struct kefir_mem *,
                                                   struct kefir_ast_type_storage *,
                                                   const char *,
                                                   struct kefir_ast_enum_type **);

#endif