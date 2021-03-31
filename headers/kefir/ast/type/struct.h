#ifndef KEFIR_AST_TYPE_STRUCT_H_
#define KEFIR_AST_TYPE_STRUCT_H_

#include "kefir/ast/type/base.h"
#include "kefir/ast/alignment.h"

typedef struct kefir_ast_struct_field {
    const char *identifier;
    const struct kefir_ast_type *type;
    struct kefir_ast_alignment *alignment;
    kefir_bool_t bitfield;
    struct kefir_ast_constant_expression *bitwidth;
} kefir_ast_struct_field_t;

typedef struct kefir_ast_struct_type {
    kefir_bool_t complete;
    const char *identifier;
    struct kefir_list fields;
    struct kefir_hashtree field_index;
} kefir_ast_struct_type_t;

const struct kefir_ast_type *kefir_ast_type_incomplete_structure(struct kefir_mem *,
                                                             struct kefir_ast_type_bundle *,
                                                             const char *);
const struct kefir_ast_type *kefir_ast_type_incomplete_union(struct kefir_mem *,
                                                         struct kefir_ast_type_bundle *,
                                                         const char *);

kefir_result_t kefir_ast_struct_type_get_field(const struct kefir_ast_struct_type *,
                                           const char *,
                                           const struct kefir_ast_struct_field **);

kefir_result_t kefir_ast_struct_type_resolve_field(const struct kefir_ast_struct_type *,
                                               const char *,
                                               const struct kefir_ast_struct_field **);

kefir_result_t kefir_ast_struct_type_field(struct kefir_mem *,
                                       struct kefir_symbol_table *,
                                       struct kefir_ast_struct_type *,
                                       const char *,
                                       const struct kefir_ast_type *,
                                       struct kefir_ast_alignment *);

kefir_result_t kefir_ast_struct_type_bitfield(struct kefir_mem *,
                                          struct kefir_symbol_table *,
                                          struct kefir_ast_struct_type *,
                                          const char *,
                                          const struct kefir_ast_type *,
                                          struct kefir_ast_alignment *,
                                          struct kefir_ast_constant_expression *);
                                    
const struct kefir_ast_type *kefir_ast_type_structure(struct kefir_mem *,
                                                  struct kefir_ast_type_bundle *,
                                                  const char *,
                                                  struct kefir_ast_struct_type **);
const struct kefir_ast_type *kefir_ast_type_union(struct kefir_mem *,
                                              struct kefir_ast_type_bundle *,
                                              const char *,
                                              struct kefir_ast_struct_type **);

#endif