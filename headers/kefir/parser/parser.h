#ifndef KEFIR_PARSER_PARSER_H_
#define KEFIR_PARSER_PARSER_H_

#include "kefir/core/mem.h"
#include "kefir/parser/lexem_stream.h"
#include "kefir/ast/node.h"

#define KEFIR_PARSER_LOOKAHEAD 1

typedef struct kefir_parser kefir_parser_t;

typedef struct kefir_parser {
    struct kefir_symbol_table *symbols;
    struct kefir_token_stream *tokens;
    struct kefir_token lookahead[KEFIR_PARSER_LOOKAHEAD];
} kefir_parser_t;

#define KEFIR_PARSER_RULE_MATCH(_parser, _rule) (kefir_parser_match_rule_##_rule((_parser)))
#define KEFIR_PARSER_RULE_REDUCE(_mem, _parser, _rule, _result) \
    (kefir_parser_reduce_rule_##_rule((_mem), (_parser), (_result)))

kefir_result_t kefir_parser_init(struct kefir_mem *, struct kefir_parser *, struct kefir_symbol_table *,
                                 struct kefir_token_stream *);
kefir_result_t kefir_parser_free(struct kefir_mem *, struct kefir_parser *);
kefir_result_t kefir_parser_shift(struct kefir_mem *, struct kefir_parser *);
kefir_result_t kefir_parser_next(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_node_base **);

#endif