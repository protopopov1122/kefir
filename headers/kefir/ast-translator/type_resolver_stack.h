#ifndef KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_STACK_H_
#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_STACK_H_

#include "kefir/ast-translator/type_resolver.h"
#include "kefir/core/list.h"

typedef struct kefir_ast_translator_type_resolver_stack {
    struct kefir_ast_translator_type_resolver resolver;
    struct kefir_list stack;
} kefir_ast_translator_type_resolver_stack_t;

kefir_result_t kefir_ast_translator_type_resolver_stack_init(struct kefir_mem *,
                                                         struct kefir_ast_translator_type_resolver_stack *);

struct kefir_ast_translator_type_resolver *kefir_ast_translator_type_resolver_stack_top(const struct kefir_ast_translator_type_resolver_stack *);

kefir_result_t kefir_ast_translator_type_resolver_stack_push(struct kefir_mem *,
                                                         struct kefir_ast_translator_type_resolver_stack *);

kefir_result_t kefir_ast_translator_type_resolver_stack_pop(struct kefir_mem *,
                                                        struct kefir_ast_translator_type_resolver_stack *);

#endif