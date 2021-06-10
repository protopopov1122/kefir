#ifndef KEFIR_AST_NODE_HELPERS_H_
#define KEFIR_AST_NODE_HELPERS_H_

#include "kefir/ast/node.h"

#ifndef KEFIR_AST_NODE_INTERNAL_DEF
#error "This file should only be included from kefir/ast/node.h"
#endif

kefir_result_t kefir_ast_generic_selection_append(struct kefir_mem *, struct kefir_ast_generic_selection *,
                                                  struct kefir_ast_type_name *, struct kefir_ast_node_base *);

kefir_result_t kefir_ast_function_call_append(struct kefir_mem *, struct kefir_ast_function_call *,
                                              struct kefir_ast_node_base *);

kefir_result_t kefir_ast_comma_append(struct kefir_mem *, struct kefir_ast_comma_operator *,
                                      struct kefir_ast_node_base *);

#endif
