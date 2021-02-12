#ifndef KEFIR_AST_EXPR_TYPE_H_
#define KEFIR_AST_EXPR_TYPE_H_

#include "kefir/core/mem.h"
#include "kefir/ast/node.h"
#include "kefir/ast/type.h"
#include "kefir/ast/translator/context.h"

kefir_result_t kefir_ast_assign_expression_type(struct kefir_mem *,
                                            const struct kefir_ast_translation_context *,
                                            struct kefir_ast_node_base *);

#define KEFIR_AST_ASSIGN_EXPRESSION_TYPE(mem, context, base) \
    ((base)->expression_type == NULL ? kefir_ast_assign_expression_type((mem), (context), (base)) : KEFIR_OK)

#endif