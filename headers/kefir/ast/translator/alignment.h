#ifndef KEFIR_AST_TRANSLATOR_ALIGNMENT_H_
#define KEFIR_AST_TRANSLATOR_ALIGNMENT_H_

#include "kefir/ast/alignment.h"
#include "kefir/ast/translator/environment.h"

kefir_result_t kefir_ast_translator_eval_alignment(struct kefir_mem *,
                                               const struct kefir_ast_alignment *,
                                               const struct kefir_ast_translator_environment *,
                                               kefir_size_t *);

#endif