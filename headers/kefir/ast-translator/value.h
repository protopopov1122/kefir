#ifndef KEFIR_AST_TRANSLATOR_VALUE_H_
#define KEFIR_AST_TRANSLATOR_VALUE_H_

#include "kefir/ast/type.h"
#include "kefir/ir/builder.h"

kefir_result_t kefir_ast_translator_load_value(const struct kefir_ast_type *,
                                              struct kefir_irbuilder_block *);

#endif