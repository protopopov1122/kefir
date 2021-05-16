#ifndef KEFIR_AST_TYPE_ARRAY_H_
#define KEFIR_AST_TYPE_ARRAY_H_

#include "kefir/ast/constant_expression.h"
#include "kefir/ast/type/base.h"
#include "kefir/ast/constants.h"

typedef enum kefir_ast_array_boundary_type {
    KEFIR_AST_ARRAY_UNBOUNDED,
    KEFIR_AST_ARRAY_BOUNDED,
    KEFIR_AST_ARRAY_BOUNDED_STATIC,
    KEFIR_AST_ARRAY_VLA,
    KEFIR_AST_ARRAY_VLA_STATIC
} kefir_ast_array_boundary_type_t;

typedef struct kefir_ast_array_type {
    const struct kefir_ast_type *element_type;
    kefir_ast_array_boundary_type_t boundary;
    struct kefir_ast_type_qualification qualifications;
    union {
        struct kefir_ast_constant_expression *const_length;
        struct kefir_ast_node_base *vla_length;
    };
} kefir_ast_array_type_t;

const struct kefir_ast_type *kefir_ast_type_unbounded_array(struct kefir_mem *,
                                                        struct kefir_ast_type_bundle *,
                                                        const struct kefir_ast_type *,
                                                        const struct kefir_ast_type_qualification *);

const struct kefir_ast_type *kefir_ast_type_array(struct kefir_mem *,
                                              struct kefir_ast_type_bundle *,
                                              const struct kefir_ast_type *,
                                              struct kefir_ast_constant_expression *,
                                              const struct kefir_ast_type_qualification *);
                                              
const struct kefir_ast_type *kefir_ast_type_array_static(struct kefir_mem *,
                                              struct kefir_ast_type_bundle *,
                                              const struct kefir_ast_type *,
                                              struct kefir_ast_constant_expression *,
                                              const struct kefir_ast_type_qualification *);

const struct kefir_ast_type *kefir_ast_type_vlen_array(struct kefir_mem *,
                                                   struct kefir_ast_type_bundle *,
                                                   const struct kefir_ast_type *,
                                                   struct kefir_ast_node_base *,
                                                   const struct kefir_ast_type_qualification *);
                                              
const struct kefir_ast_type *kefir_ast_type_vlen_array_static(struct kefir_mem *,
                                                          struct kefir_ast_type_bundle *,
                                                          const struct kefir_ast_type *,
                                                          struct kefir_ast_node_base *,
                                                          const struct kefir_ast_type_qualification *);

kefir_size_t kefir_ast_type_array_const_length(const struct kefir_ast_array_type *);

#endif
