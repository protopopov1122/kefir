#ifndef KEFIR_AST_ANALYZER_STRUCTURE_TRAVERSAL_H_
#define KEFIR_AST_ANALYZER_STRUCTURE_TRAVERSAL_H_

#include "kefir/core/list.h"
#include "kefir/core/mem.h"
#include "kefir/ast/type.h"
#include "kefir/ast/designator.h"

typedef enum kefir_ast_type_traversal_layer_type {
    KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE,
    KEFIR_AST_TYPE_TRAVERSAL_UNION,
    KEFIR_AST_TYPE_TRAVERSAL_ARRAY,
    KEFIR_AST_TYPE_TRAVERSAL_SCALAR
} kefir_ast_type_traversal_layer_type_t;

typedef struct kefir_ast_type_traversal_structure {
    const struct kefir_list_entry *iterator;
} kefir_ast_type_traversal_structure_t;

typedef struct kefir_ast_type_traversal_array {
    kefir_size_t index;
} kefir_ast_type_traversal_array_t;

typedef struct kefir_ast_type_traversal_layer {
    const struct kefir_ast_type_traversal_layer *parent;
    kefir_ast_type_traversal_layer_type_t type;
    const struct kefir_ast_type *object_type;
    kefir_bool_t init;
    union {
        struct kefir_ast_type_traversal_structure structure;
        struct kefir_ast_type_traversal_array array;
    };
} kefir_ast_type_traversal_layer_t;

typedef struct kefir_ast_type_traversal {
    struct kefir_list stack;
    const struct kefir_ast_type *current_object_type;
} kefir_ast_type_traversal_t;

kefir_result_t kefir_ast_type_traversal_init(struct kefir_mem *,
                                         struct kefir_ast_type_traversal *,
                                         const struct kefir_ast_type *);

kefir_result_t kefir_ast_type_traversal_free(struct kefir_mem *,
                                         struct kefir_ast_type_traversal *);

kefir_result_t kefir_ast_type_traversal_next(struct kefir_mem *,
                                         struct kefir_ast_type_traversal *,
                                         const struct kefir_ast_type **,
                                         const struct kefir_ast_type_traversal_layer **);

kefir_result_t kefir_ast_type_traversal_next_recursive(struct kefir_mem *,
                                                   struct kefir_ast_type_traversal *,
                                                   const struct kefir_ast_type **,
                                                   const struct kefir_ast_type_traversal_layer **);

kefir_result_t kefir_ast_type_traversal_next_recursive2(struct kefir_mem *,
                                                    struct kefir_ast_type_traversal *,
                                                    kefir_bool_t (*)(const struct kefir_ast_type *, void *),
                                                    void *,
                                                    const struct kefir_ast_type **,
                                                    const struct kefir_ast_type_traversal_layer **);

kefir_result_t kefir_ast_type_traversal_navigate(struct kefir_mem *,
                                             struct kefir_ast_type_traversal *,
                                             const struct kefir_ast_designator *);

kefir_bool_t kefir_ast_type_traversal_empty(struct kefir_ast_type_traversal *);

#endif