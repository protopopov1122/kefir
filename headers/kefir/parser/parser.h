#ifndef KEFIR_PARSER_PARSER_H_
#define KEFIR_PARSER_PARSER_H_

#include "kefir/core/mem.h"
#include "kefir/parser/cursor.h"
#include "kefir/ast/node.h"

typedef struct kefir_parser kefir_parser_t;

typedef struct kefir_parser {
    struct kefir_symbol_table *symbols;
    struct kefir_parser_token_cursor *cursor;
} kefir_parser_t;

typedef kefir_result_t (*kefir_parser_rule_fn_t)(struct kefir_mem *, struct kefir_parser *,
                                                 struct kefir_ast_node_base **, void *);

kefir_result_t kefir_parser_init(struct kefir_mem *, struct kefir_parser *, struct kefir_symbol_table *,
                                 struct kefir_parser_token_cursor *);
kefir_result_t kefir_parser_free(struct kefir_mem *, struct kefir_parser *);
kefir_result_t kefir_parser_apply(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_node_base **,
                                  kefir_parser_rule_fn_t, void *);

#define KEFIR_PARSER_RULE_FN(_rule) kefir_parser_apply_rule_##_rule
#define KEFIR_PARSER_RULE_APPLY(_mem, _parser, _rule, _result) \
    (kefir_parser_apply((_mem), (_parser), (_result), KEFIR_PARSER_RULE_FN(_rule), NULL))
#define KEFIR_PARSER_NEXT_EXPRESSION(_mem, _parser, _result) \
    KEFIR_PARSER_RULE_APPLY((_mem), (_parser), assignment_expression, (_result))

#endif