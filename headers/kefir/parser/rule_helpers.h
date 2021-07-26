#ifndef KEFIR_PARSER_RULE_HELPERS_H_
#define KEFIR_PARSER_RULE_HELPERS_H_

#include "kefir/parser/parser.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

#define DEFINE_MATCH(_id)                                                                              \
    static kefir_result_t MATCH_##_id(struct kefir_mem *mem, const struct kefir_parser *parser,        \
                                      const struct kefir_parser_rule *rule) {                          \
        REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator")); \
        REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));        \
        REQUIRE(rule != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser rule"));     \
        do

#define DEFINE_REDUCE(_id)                                                                                          \
    static kefir_result_t REDUCE_##_id(struct kefir_mem *mem, struct kefir_parser *parser,                          \
                                       const struct kefir_parser_rule *rule, struct kefir_ast_node_base **result) { \
        REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));              \
        REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));                     \
        REQUIRE(rule != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser rule"));                  \
        REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST node"));        \
        REQUIRE_OK(MATCH_##_id(mem, parser, rule));                                                                 \
        do

#define END_PROCEDURE \
    while (0)         \
        ;             \
    return KEFIR_OK;  \
    }

#define DEFINE_RULE(_id)                                       \
    const struct kefir_parser_rule KEFIR_PARSER_RULE_##_id = { \
        .match = MATCH_##_id, .reduce = REDUCE_##_id, .payload = NULL}

#define PARSER_LOOKAHEAD(_parser, _idx) (&(_parser)->lookahead[(_idx)])
#define PARSER_TOKEN_IS(_parser, _idx, _klass) (PARSER_LOOKAHEAD((_parser), (_idx))->klass == (_klass))
#define PARSER_TOKEN_IS_SENTINEL(_parser, _idx) PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_SENTINEL)
#define PARSER_TOKEN_IS_KEYWORD(_parser, _idx, _keyword)        \
    (PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_KEYWORD) && \
     PARSER_LOOKAHEAD((_parser), (_idx))->keyword == (_keyword))
#define PARSER_TOKEN_IS_IDENTIFIER(_parser, _idx) PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_IDENTIFIER)
#define PARSER_TOKEN_IS_CONSTANT(_parser, _idx, _type)           \
    (PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_CONSTANT) && \
     PARSER_LOOKAHEAD((_parser), (_idx))->constant.type == (_type))
#define PARSER_TOKEN_IS_STRING_LITERAL(_parser, _idx) PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_STRING_LITERAL)
#define PARSER_TOKEN_IS_PUNCTUATOR(_parser, _idx, _punctuator)     \
    (PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_PUNCTUATOR) && \
     PARSER_LOOKAHEAD((_parser), (_idx))->punctuator == (_punctuator))

#define PARSER_SHIFT(_count) REQUIRE_OK(kefir_parser_shift(mem, parser, (_count)))

#define PARSER_MATCH_REDUCE(_res, _rule, _result)                              \
    do {                                                                       \
        *(_res) = KEFIR_PARSER_RULE_MATCH(mem, parser, _rule);                 \
        if (*(_res) == KEFIR_OK) {                                             \
            REQUIRE_OK(KEFIR_PARSER_RULE_REDUCE(mem, parser, _rule, _result)); \
        } else {                                                               \
            REQUIRE(*(_res) == KEFIR_NO_MATCH, *(_res));                       \
        }                                                                      \
    } while (0)

#define PARSER_MATCH_REDUCE_ELSE(_res, _rule, _result) \
    if (*(_res) == KEFIR_NO_MATCH) {                   \
        PARSER_MATCH_REDUCE((_res), _rule, (_result)); \
    } else {                                           \
        REQUIRE_OK(*(_res));                           \
    }

#define ALLOC_NODE(_ptr, _proc, ...)                                                                      \
    do {                                                                                                  \
        *(_ptr) = KEFIR_AST_NODE_BASE(_proc(__VA_ARGS__));                                                \
        REQUIRE(*(_ptr) != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST node")); \
    } while (0)

#endif
