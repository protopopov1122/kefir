#ifndef KEFIR_AST_INITIALIZER_TRAVERSAL_H_
#define KEFIR_AST_INITIALIZER_TRAVERSAL_H_

#include "kefir/ast/initializer.h"
#include "kefir/ast/context.h"

typedef struct kefir_ast_initializer_traversal {
    kefir_result_t (*visit_value)(const struct kefir_ast_designator *, struct kefir_ast_node_base *, void *);
    kefir_result_t (*visit_string_literal)(const struct kefir_ast_designator *, struct kefir_ast_node_base *,
                                           const char *, kefir_size_t, void *);
    kefir_result_t (*begin_struct_union)(const struct kefir_ast_designator *, const struct kefir_ast_initializer *,
                                         void *);
    kefir_result_t (*begin_array)(const struct kefir_ast_designator *, const struct kefir_ast_initializer *, void *);
    kefir_result_t (*visit_initializer_list)(const struct kefir_ast_designator *, const struct kefir_ast_initializer *,
                                             void *);
    void *payload;
} kefir_ast_initializer_traversal_t;

#define KEFIR_AST_INITIALIZER_TRAVERSAL_INIT(_traversal) \
    do {                                                 \
        (_traversal)->visit_value = NULL;                \
        (_traversal)->visit_string_literal = NULL;       \
        (_traversal)->begin_struct_union = NULL;         \
        (_traversal)->begin_array = NULL;                \
        (_traversal)->visit_initializer_list = NULL;     \
        (_traversal)->payload = NULL;                    \
    } while (0)

kefir_result_t kefi_ast_traverse_initializer(struct kefir_mem *, const struct kefir_ast_context *,
                                             const struct kefir_ast_initializer *, const struct kefir_ast_type *,
                                             const struct kefir_ast_initializer_traversal *);

#endif
