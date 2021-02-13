#ifndef KEFIR_AST_TRANSLATOR_TRANSLATOR_H_
#define KEFIR_AST_TRANSLATOR_TRANSLATOR_H_

#include "kefir/core/mem.h"
#include "kefir/ast/node.h"
#include "kefir/ir/builder.h"

kefir_result_t kefir_ast_translate_type(const struct kefir_ast_type *,
                                    struct kefir_irbuilder_type *,
                                    const kefir_id_t *);

kefir_result_t kefir_ast_translate_expression(const struct kefir_ast_node_base *,
                                          struct kefir_irbuilder_block *);

#endif