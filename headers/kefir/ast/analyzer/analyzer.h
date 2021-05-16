#ifndef KEFIR_AST_ANALYZER_ANALYZER_H_
#define KEFIR_AST_ANALYZER_ANALYZER_H_

#include "kefir/core/mem.h"
#include "kefir/ast/node.h"
#include "kefir/ast/context.h"
#include "kefir/ast/alignment.h"

typedef enum kefir_ast_type_analysis_context {
    KEFIR_AST_TYPE_ANALYSIS_DEFAULT,
    KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER
} kefir_ast_type_analysis_context_t;

kefir_result_t kefir_ast_analyze_constant_expression(struct kefir_mem *,
                                                 const struct kefir_ast_context *,
                                                 struct kefir_ast_constant_expression *);

kefir_result_t kefir_ast_analyze_alignment(struct kefir_mem *,
                                       const struct kefir_ast_context *,
                                       struct kefir_ast_alignment *);

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

kefir_result_t kefir_ast_analyze_type(struct kefir_mem *,
                                  const struct kefir_ast_context *,
                                  kefir_ast_type_analysis_context_t,
                                  const struct kefir_ast_type *);

#endif
