#ifndef KEFIR_AST_TYPE_STRUCT_H_
#define KEFIR_AST_TYPE_STRUCT_H_

#include "kefir/ast/type/base.h"

typedef struct kefir_ast_struct_field {
    const char *identifier;
    const struct kefir_ast_type *type;
    kefir_bool_t bitfield;
    kefir_size_t bitwidth;
} kefir_ast_struct_field_t;

typedef struct kefir_ast_struct_type {
    kefir_bool_t complete;
    const char *identifier;
    struct kefir_list fields;
    struct kefir_hashtree field_index;
} kefir_ast_struct_type_t;

const struct kefir_ast_type *kefir_ast_type_incomplete_structure(struct kefir_mem *,
                                                             struct kefir_ast_type_storage *,
                                                             const char *);
const struct kefir_ast_type *kefir_ast_type_incomplete_union(struct kefir_mem *,
                                                         struct kefir_ast_type_storage *,
                                                         const char *);

kefir_result_t kefir_ast_struct_type_get_field(const struct kefir_ast_struct_type *,
                                           const char *,
                                           const struct kefir_ast_struct_field **);

kefir_result_t kefir_ast_struct_type_field(struct kefir_mem *,
                                       struct kefir_symbol_table *,
                                       struct kefir_ast_struct_type *,
                                       const char *,
                                       const struct kefir_ast_type *);

kefir_result_t kefir_ast_struct_type_bitfield(struct kefir_mem *,
                                          struct kefir_symbol_table *,
                                          struct kefir_ast_struct_type *,
                                          const char *,
                                          const struct kefir_ast_type *,
                                          kefir_size_t);
                                    
const struct kefir_ast_type *kefir_ast_type_structure(struct kefir_mem *,
                                                  struct kefir_ast_type_storage *,
                                                  const char *,
                                                  struct kefir_ast_struct_type **);
const struct kefir_ast_type *kefir_ast_type_union(struct kefir_mem *,
                                              struct kefir_ast_type_storage *,
                                              const char *,
                                              struct kefir_ast_struct_type **);

#endif