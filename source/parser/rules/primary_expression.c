#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

DEFINE_MATCH(PRIMARY_EXPRESSION) {
    REQUIRE(KEFIR_PARSER_RULE_MATCH(mem, parser, identifier) == KEFIR_OK ||
                KEFIR_PARSER_RULE_MATCH(mem, parser, constant) == KEFIR_OK ||
                KEFIR_PARSER_RULE_MATCH(mem, parser, string_literal) == KEFIR_OK ||
                KEFIR_PARSER_RULE_MATCH(mem, parser, generic_selection) == KEFIR_OK ||
                PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
}
END_PROCEDURE

DEFINE_REDUCE(PRIMARY_EXPRESSION) {
    kefir_result_t res = KEFIR_OK;
    PARSER_MATCH_REDUCE(&res, identifier, result);
    PARSER_MATCH_REDUCE_ELSE(&res, constant, result);
    PARSER_MATCH_REDUCE_ELSE(&res, string_literal, result);
    PARSER_MATCH_REDUCE_ELSE(&res, generic_selection, result);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
        PARSER_SHIFT(1);
        PARSER_MATCH_REDUCE(&res, primary_expression, result);
        REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
        PARSER_SHIFT(1);
    } else {
        REQUIRE_OK(res);
    }
}
END_PROCEDURE

DEFINE_RULE(PRIMARY_EXPRESSION);
