#ifndef KEFIR_PARSER_RULE_HELPERS_H_
#define KEFIR_PARSER_RULE_HELPERS_H_

#include "kefir/parser/parser.h"
#include "kefir/parser/rules.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

#define REDUCE_PROLOGUE(_mem, _parser, _result, _id)                                                                   \
    do {                                                                                                               \
        REQUIRE((_mem) != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));              \
        REQUIRE((_parser) != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));                     \
        REQUIRE((_result) != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST node"));        \
        REQUIRE(KEFIR_PARSER_RULE_MATCH((_parser), _id), KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match parser rule")); \
    } while (0)

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

#define REQUIRE_ALLOC(_ptr, _expr, _error)                                           \
    do {                                                                             \
        *(_ptr) = (_expr);                                                           \
        REQUIRE(*(_ptr) != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, (_error))); \
    } while (0)

#endif
