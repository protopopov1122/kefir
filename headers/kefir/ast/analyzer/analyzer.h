#ifndef KEFIR_AST_ANALYZER_ANALYZER_H_
#define KEFIR_AST_ANALYZER_ANALYZER_H_

#include "kefir/core/mem.h"
#include "kefir/ast/node.h"
#include "kefir/ast/context.h"

kefir_result_t kefir_ast_node_is_lvalue_reference_constant(const struct kefir_ast_context *,
                                                       const struct kefir_ast_node_base *,
                                                       kefir_bool_t *);

kefir_result_t kefir_ast_node_assignable(struct kefir_mem *,
                                     const struct kefir_ast_context *,
                                     const struct kefir_ast_node_base *,
                                     const struct kefir_ast_type *);

kefir_result_t kefir_ast_analyze_node(struct kefir_mem *,
                                  const struct kefir_ast_context *,
                                  struct kefir_ast_node_base *);

#endif