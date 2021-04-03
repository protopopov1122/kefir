#ifndef KEFIR_AST_TRANSLATOR_TRANSLATOR_H_
#define KEFIR_AST_TRANSLATOR_TRANSLATOR_H_

#include "kefir/core/mem.h"
#include "kefir/ast/node.h"
#include "kefir/ast/type_layout.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ir/builder.h"

kefir_result_t kefir_ast_translate_object_type(struct kefir_mem *,
                                           const struct kefir_ast_type *,
                                           kefir_size_t,
                                           const struct kefir_ast_translator_environment *,
                                           struct kefir_irbuilder_type *,
                                           struct kefir_ast_type_layout **);

kefir_result_t kefir_ast_translate_expression(const struct kefir_ast_node_base *,
                                          struct kefir_irbuilder_block *);

#endif