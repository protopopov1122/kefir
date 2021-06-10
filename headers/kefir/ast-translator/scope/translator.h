#ifndef KEFIR_AST_TRANSLATOR_SCOPE_TRANSLATOR_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_TRANSLATOR_H_

#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ir/module.h"

kefir_result_t kefir_ast_translate_global_scope(struct kefir_mem *, struct kefir_ir_module *,
                                                const struct kefir_ast_translator_global_scope_layout *);

#endif
