#ifndef KEFIR_AST_ANALYZER_INITIALIZER_H_
#define KEFIR_AST_ANALYZER_INITIALIZER_H_

#include "kefir/ast/initializer.h"
#include "kefir/ast/type.h"
#include "kefir/ast/context.h"

kefir_result_t kefir_ast_analyze_initializer(struct kefir_mem *,
                                         const struct kefir_ast_context *,
                                         const struct kefir_ast_type *,
                                         const struct kefir_ast_initializer *);

#endif