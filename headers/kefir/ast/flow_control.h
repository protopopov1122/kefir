#ifndef KEFIR_AST_FLOW_CONTROL_H_
#define KEFIR_AST_FLOW_CONTROL_H_

#include "kefir/ast/base.h"
#include "kefir/core/tree.h"

typedef struct kefir_ast_flow_control_statement kefir_ast_flow_control_statement_t;

#define KEFIR_AST_FLOW_CONTROL_STATEMENT_PAYLOAD_SIZE (sizeof(kefir_uptr_t) * 4)

typedef enum kefir_ast_flow_control_statement_type {
    KEFIR_AST_FLOW_CONTROL_STATEMENT_IF,
    KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH,
    KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR,
    KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE,
    KEFIR_AST_FLOW_CONTROL_STATEMENT_DO
} kefir_ast_flow_control_statement_type_t;

typedef struct kefir_ast_flow_control_statement_cleanup {
    kefir_result_t (*callback)(struct kefir_mem *, struct kefir_ast_flow_control_statement *, void *);
    void *payload;
} kefir_ast_flow_control_statement_cleanup_t;

typedef struct kefir_ast_flow_control_statement {
    const struct kefir_ast_flow_control_statement *parent;
    kefir_ast_flow_control_statement_type_t type;
    struct kefir_ast_flow_control_statement_cleanup cleanup;
    struct {
        unsigned char content[KEFIR_AST_FLOW_CONTROL_STATEMENT_PAYLOAD_SIZE];
        void *ptr;
        struct kefir_ast_flow_control_statement_cleanup *cleanup;
    } payload;
} kefir_ast_flow_control_statement_t;

typedef struct kefir_ast_flow_control_tree {
    struct kefir_tree_node root;
    struct kefir_tree_node *current;
} kefir_ast_flow_control_tree_t;

#define KEFIR_AST_FLOW_CONTROL_STATEMENT_SET_CLEANUP(_flow_control, _callback, _payload) \
    do {                                                                                 \
        (_flow_control)->payload.cleanup->callback = (_callback);                        \
        (_flow_control)->payload.cleanup->payload = (_payload);                          \
    } while (0)

kefir_result_t kefir_ast_flow_control_tree_init(struct kefir_ast_flow_control_tree *);
kefir_result_t kefir_ast_flow_control_tree_free(struct kefir_mem *, struct kefir_ast_flow_control_tree *);
kefir_result_t kefir_ast_flow_control_tree_push(struct kefir_mem *, struct kefir_ast_flow_control_tree *,
                                                kefir_ast_flow_control_statement_type_t,
                                                const struct kefir_ast_flow_control_statement **);
kefir_result_t kefir_ast_flow_control_tree_pop(struct kefir_ast_flow_control_tree *);
kefir_result_t kefir_ast_flow_control_tree_top(struct kefir_ast_flow_control_tree *,
                                               const struct kefir_ast_flow_control_statement **);
kefir_result_t kefir_ast_flow_control_tree_traverse(struct kefir_ast_flow_control_tree *,
                                                    kefir_result_t (*)(const struct kefir_ast_flow_control_statement *,
                                                                       void *, kefir_bool_t *),
                                                    void *, const struct kefir_ast_flow_control_statement **);

#endif
