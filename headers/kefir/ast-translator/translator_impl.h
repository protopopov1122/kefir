#ifndef KEFIR_AST_TRANSLATOR_TRANSLATOR_IMPL_H_
#define KEFIR_AST_TRANSLATOR_TRANSLATOR_IMPL_H_

#include "kefir/ast-translator/context.h"
#include "kefir/ast/node.h"
#include "kefir/ir/builder.h"

#define NODE_TRANSLATOR(_id, _type) \
    kefir_result_t kefir_ast_translate_##_id##_node(struct kefir_mem *, \
                                                const struct kefir_ast_translator_context *, \
                                                struct kefir_irbuilder_block *, \
                                                const _type *)

NODE_TRANSLATOR(constant, struct kefir_ast_constant);
NODE_TRANSLATOR(identifier, struct kefir_ast_identifier);
NODE_TRANSLATOR(generic_selection, struct kefir_ast_generic_selection);
NODE_TRANSLATOR(array_subscript, struct kefir_ast_array_subscript);
NODE_TRANSLATOR(unary_operation, struct kefir_ast_unary_operation);
NODE_TRANSLATOR(binary_operation, struct kefir_ast_binary_operation);
#undef NODE_TRANSLATOR

#endif