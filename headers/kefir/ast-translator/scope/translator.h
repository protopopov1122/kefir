#ifndef KEFIR_AST_TRANSLATOR_SCOPE_TRANSLATOR_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_TRANSLATOR_H_

#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast/context.h"
#include "kefir/ir/module.h"

kefir_result_t kefir_ast_translate_global_scope(struct kefir_mem *, const struct kefir_ast_context *,
                                                struct kefir_ir_module *,
                                                const struct kefir_ast_translator_global_scope_layout *);

kefir_result_t kefir_ast_translate_data_initializer(struct kefir_mem *, const struct kefir_ast_context *,
                                                    struct kefir_ir_module *, struct kefir_ast_type_layout *,
                                                    const struct kefir_ir_type *, const struct kefir_ast_initializer *,
                                                    struct kefir_ir_data *, kefir_size_t);

#endif
