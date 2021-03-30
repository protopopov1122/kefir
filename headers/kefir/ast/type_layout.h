#ifndef KEFIR_AST_ANALYZER_TYPE_LAYOUT_H_
#define KEFIR_AST_ANALYZER_TYPE_LAYOUT_H_

#include "kefir/core/hashtree.h"
#include "kefir/ast/type.h"

typedef struct kefir_ast_type_layout kefir_ast_type_layout_t;

typedef struct kefir_ast_struct_type_layout {
    struct kefir_hashtree members;
} kefir_ast_struct_type_layout_t;

typedef struct kefir_ast_array_type_layout {
    struct kefir_ast_type_layout *element_type;
} kefir_ast_array_type_layout_t;

typedef struct kefir_ast_type_layout {
    const struct kefir_ast_type *type;
    kefir_uptr_t value;

    union {
        struct kefir_ast_struct_type_layout structure_layout;
        struct kefir_ast_array_type_layout array_layout;
    };
} kefir_ast_type_layout_t;

struct kefir_ast_type_layout *kefir_ast_new_type_layout(struct kefir_mem *,
                                                    const struct kefir_ast_type *,
                                                    kefir_uptr_t);

kefir_result_t kefir_ast_type_layout_free(struct kefir_mem *,
                                      struct kefir_ast_type_layout *);

#endif