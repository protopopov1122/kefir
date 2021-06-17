#ifndef KEFIR_AST_TRANSLATOR_FLOW_CONTROL_H_
#define KEFIR_AST_TRANSLATOR_FLOW_CONTROL_H_

#include "kefir/core/list.h"
#include "kefir/ast/flow_control.h"
#include "kefir/ir/module.h"

typedef struct kefir_ast_translator_flow_control_point {
    kefir_bool_t resolved;
    union {
        kefir_uint64_t address;
        struct kefir_list dependents;
    };
} kefir_ast_translator_flow_control_point_t;

kefir_result_t kefir_ast_translator_flow_control_point_init(struct kefir_mem *, struct kefir_ast_flow_control_point *,
                                                            struct kefir_ast_translator_flow_control_point **);

kefir_result_t kefir_ast_translator_flow_control_point_reference(struct kefir_mem *,
                                                                 struct kefir_ast_flow_control_point *,
                                                                 struct kefir_irblock *, kefir_size_t);

kefir_result_t kefir_ast_translator_flow_control_point_resolve(struct kefir_mem *,
                                                               struct kefir_ast_flow_control_point *, kefir_uint64_t);

kefir_result_t kefir_ast_translator_flow_control_tree_init(struct kefir_mem *, struct kefir_ast_flow_control_tree *);

#endif
