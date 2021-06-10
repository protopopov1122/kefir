#ifndef KEFIR_AST_TRANSLATOR_SCOPE_SCOPE_LAYOUT_IMPL_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_SCOPE_LAYOUT_IMPL_H_

#include "kefir/ast-translator/scope/scoped_identifier.h"

kefir_result_t kefir_ast_translator_scoped_identifer_payload_free(struct kefir_mem *,
                                                                  struct kefir_ast_scoped_identifier *, void *);

kefir_result_t kefir_ast_translator_scoped_identifier_remove(struct kefir_mem *, struct kefir_list *,
                                                             struct kefir_list_entry *, void *);

kefir_result_t kefir_ast_translator_scoped_identifier_insert(struct kefir_mem *, const char *,
                                                             const struct kefir_ast_scoped_identifier *,
                                                             struct kefir_list *);

#endif
