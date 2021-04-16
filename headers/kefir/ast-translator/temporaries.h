#ifndef KEFIR_AST_TRANSLATOR_TEMPORARIES_H_
#define KEFIR_AST_TRANSLATOR_TEMPORARIES_H_

#include "kefir/ast/temporaries.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ir/builder.h"

kefir_result_t kefir_ast_translator_fetch_temporary(struct kefir_mem *,
                                                struct kefir_ast_translator_context *,
                                                struct kefir_irbuilder_block *,
                                                const struct kefir_ast_temporary_identifier *);

#endif