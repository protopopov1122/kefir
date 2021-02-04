#ifndef KEFIR_AST_TRANSLATOR_TYPECONV_H_
#define KEFIR_AST_TRANSLATOR_TYPECONV_H_

#include "kefir/ast/type.h"
#include "kefir/ir/builder.h"

kefir_result_t kefir_ast_translate_typeconv(struct kefir_irbuilder_block *,
                                        const struct kefir_ast_type *,
                                        const struct kefir_ast_type *);

#endif