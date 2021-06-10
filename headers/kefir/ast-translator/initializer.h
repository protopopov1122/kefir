#ifndef KEFIR_AST_TRANSLATOR_INITIALIZER_H_
#define KEFIR_AST_TRANSLATOR_INITIALIZER_H_

#include "kefir/ast/initializer.h"
#include "kefir/ir/builder.h"
#include "kefir/ast-translator/context.h"

kefir_result_t kefir_ast_translate_initializer(struct kefir_mem *, struct kefir_ast_translator_context *,
                                               struct kefir_irbuilder_block *, const struct kefir_ast_type *,
                                               const struct kefir_ast_initializer *);

#endif
