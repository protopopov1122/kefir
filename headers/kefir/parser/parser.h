#ifndef KEFIR_PARSER_PARSER_H_
#define KEFIR_PARSER_PARSER_H_

#include "kefir/core/mem.h"
#include "kefir/parser/lexem_stream.h"
#include "kefir/ast/node.h"

#define KEFIR_PARSER_LOOKAHEAD 1

typedef struct kefir_parser kefir_parser_t;

typedef struct kefir_parser_rule {
    kefir_result_t (*match)(struct kefir_mem *, const struct kefir_parser *, const struct kefir_parser_rule *);
    kefir_result_t (*reduce)(struct kefir_mem *, struct kefir_parser *, const struct kefir_parser_rule *,
                             struct kefir_ast_node_base **);
    void *payload;
} kefir_parser_rule_t;

typedef struct kefir_parser_ruleset {
    const struct kefir_parser_rule *identifier;
    const struct kefir_parser_rule *constant;
    const struct kefir_parser_rule *string_literal;
    const struct kefir_parser_rule *generic_selection;
    const struct kefir_parser_rule *primary_expression;
} kefir_parser_ruleset_t;

typedef struct kefir_parser {
    struct kefir_symbol_table *symbols;
    struct kefir_token_stream *tokens;
    struct kefir_token lookahead[KEFIR_PARSER_LOOKAHEAD];
    const struct kefir_parser_ruleset *ruleset;
} kefir_parser_t;

#define KEFIR_PARSER_RULE_MATCH(_mem, _parser, _rule)                                     \
    ((_parser)->ruleset->_rule->match != NULL                                             \
         ? (_parser)->ruleset->_rule->match((_mem), (_parser), (_parser)->ruleset->_rule) \
         : KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Matcher for a rule is not defined"))
#define KEFIR_PARSER_RULE_REDUCE(_mem, _parser, _rule, _result)                                       \
    ((_parser)->ruleset->_rule->reduce != NULL                                                        \
         ? (_parser)->ruleset->_rule->reduce((_mem), (_parser), (_parser)->ruleset->_rule, (_result)) \
         : KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Reducer for a rule is not defined"))

kefir_result_t kefir_parser_init(struct kefir_mem *, struct kefir_parser *, const struct kefir_parser_ruleset *,
                                 struct kefir_symbol_table *, struct kefir_token_stream *);
kefir_result_t kefir_parser_free(struct kefir_mem *, struct kefir_parser *);
kefir_result_t kefir_parser_shift(struct kefir_mem *, struct kefir_parser *, kefir_uint_t);
kefir_result_t kefir_parser_next(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_node_base **);

#endif