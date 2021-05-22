#ifndef KEFIR_AST_ANALYZER_DECLARATOR_H_
#define KEFIR_AST_ANALYZER_DECLARATOR_H_

#include "kefir/ast/declarator.h"
#include "kefir/ast/type.h"
#include "kefir/ast/context.h"
#include "kefir/ast/alignment.h"

kefir_result_t kefir_ast_analyze_declaration(struct kefir_mem *,
                                         struct kefir_ast_context *,
                                         const struct kefir_ast_declarator_specifier_list *,
                                         const struct kefir_ast_declarator *,
                                         const struct kefir_ast_type **,
                                         kefir_ast_scoped_identifier_storage_t *,
                                         kefir_ast_function_specifier_t *,
                                         struct kefir_ast_alignment **);


#endif
