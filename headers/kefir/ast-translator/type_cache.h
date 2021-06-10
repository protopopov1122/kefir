#ifndef KEFIR_AST_TRANSLATOR_TYPE_CACHE_H_
#define KEFIR_AST_TRANSLATOR_TYPE_CACHE_H_

#include "kefir/core/hashtree.h"
#include "kefir/ast-translator/type_resolver.h"

typedef struct kefir_ast_translator_type_cache {
    struct kefir_ast_translator_type_resolver resolver;
    const struct kefir_ast_translator_type_resolver *parent_resolver;
    struct kefir_hashtree cache;
} kefir_ast_translator_type_cache_t;

kefir_result_t kefir_ast_translator_type_cache_init(struct kefir_ast_translator_type_cache *,
                                                    const struct kefir_ast_translator_type_resolver *);

#endif
