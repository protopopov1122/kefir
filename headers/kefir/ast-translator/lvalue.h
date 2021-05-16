#ifndef KEFIR_AST_TRANSLATOR_LVALUE_H_
#define KEFIR_AST_TRANSLATOR_LVALUE_H_

#include "kefir/core/mem.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ir/builder.h"
#include "kefir/ast/scope.h"

kefir_result_t kefir_ast_translator_object_lvalue(struct kefir_mem *,
                                              struct kefir_ast_translator_context *,
                                              struct kefir_irbuilder_block *,
                                              const char *,
                                              const struct kefir_ast_scoped_identifier *);

kefir_result_t kefir_ast_translator_function_lvalue(struct kefir_mem *,
                                                struct kefir_ast_translator_context *,
                                                struct kefir_irbuilder_block *,
                                                const char *);

kefir_result_t kefir_ast_translate_array_subscript_lvalue(struct kefir_mem *,
                                                      struct kefir_ast_translator_context *,
                                                      struct kefir_irbuilder_block *,
                                                      const struct kefir_ast_array_subscript *);

kefir_result_t kefir_ast_translate_struct_member_lvalue(struct kefir_mem *,
                                                    struct kefir_ast_translator_context *,
                                                    struct kefir_irbuilder_block *,
                                                    const struct kefir_ast_struct_member *);

kefir_result_t kefir_ast_translate_compound_literal_lvalue(struct kefir_mem *,
                                                       struct kefir_ast_translator_context *,
                                                       struct kefir_irbuilder_block *,
                                                       const struct kefir_ast_compound_literal *);

#endif
