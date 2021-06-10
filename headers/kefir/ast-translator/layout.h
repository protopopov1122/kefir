#ifndef KEFIR_AST_TRANSLATOR_LAYOUT_H_
#define KEFIR_AST_TRANSLATOR_LAYOUT_H_

#include "kefir/ast-translator/environment.h"
#include "kefir/ast/type_layout.h"

kefir_result_t kefir_ast_translator_evaluate_type_layout(struct kefir_mem *,
                                                         const struct kefir_ast_translator_environment *,
                                                         struct kefir_ast_type_layout *, const struct kefir_ir_type *);

#endif
