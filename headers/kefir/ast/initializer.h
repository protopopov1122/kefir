#ifndef KEFIR_AST_INITIALIZER_H_
#define KEFIR_AST_INITIALIZER_H_

#include "kefir/ast/base.h"
#include "kefir/core/list.h"
#include "kefir/ast/designator.h"

typedef enum kefir_ast_initializer_type {
    KEFIR_AST_INITIALIZER_EXPRESSION,
    KEFIR_AST_INITIALIZER_LIST
} kefir_ast_initializer_type_t;

typedef struct kefir_ast_initializer_list {
    struct kefir_list initializers;
} kefir_ast_initializer_list_t;

typedef struct kefir_ast_initializer {
    kefir_ast_initializer_type_t type;
    union {
        struct kefir_ast_node_base *expression;
        struct kefir_ast_initializer_list list;
    };
} kefir_ast_initializer_t;

typedef struct kefir_ast_initializer_list_entry {
    struct kefir_ast_designator *designator;
    struct kefir_ast_initializer *value;
} kefir_ast_initializer_list_entry_t;

struct kefir_ast_initializer *kefir_ast_new_expression_initializer(struct kefir_mem *,
                                                           struct kefir_ast_node_base *);
struct kefir_ast_initializer *kefir_ast_new_list_initializer(struct kefir_mem *);
kefir_result_t kefir_ast_initializer_free(struct kefir_mem *,
                                      struct kefir_ast_initializer *);

kefir_result_t kefir_ast_initializer_list_init(struct kefir_ast_initializer_list *);
kefir_result_t kefir_ast_initializer_list_free(struct kefir_mem *,
                                           struct kefir_ast_initializer_list *);
kefir_result_t kefir_ast_initializer_list_append(struct kefir_mem *,
                                             struct kefir_ast_initializer_list *,
                                             struct kefir_ast_designator *,
                                             struct kefir_ast_initializer *);

#endif