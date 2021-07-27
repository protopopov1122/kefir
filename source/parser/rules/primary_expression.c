#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_bool_t kefir_parser_match_rule_primary_expression(const struct kefir_parser *parser) {
    REQUIRE(parser != NULL, false);
    return KEFIR_PARSER_RULE_MATCH(parser, identifier) == KEFIR_OK ||
           KEFIR_PARSER_RULE_MATCH(parser, constant) == KEFIR_OK ||
           KEFIR_PARSER_RULE_MATCH(parser, string_literal) == KEFIR_OK ||
           KEFIR_PARSER_RULE_MATCH(parser, generic_selection) == KEFIR_OK ||
           PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE);
}

kefir_result_t kefir_parser_reduce_rule_primary_expression(struct kefir_mem *mem, struct kefir_parser *parser,
                                                           struct kefir_ast_node_base **result) {
    REDUCE_PROLOGUE(mem, parser, result, primary_expression);
    if (KEFIR_PARSER_RULE_MATCH(parser, identifier)) {
        REQUIRE_OK(KEFIR_PARSER_RULE_REDUCE(mem, parser, identifier, result));
    } else if (KEFIR_PARSER_RULE_MATCH(parser, constant)) {
        REQUIRE_OK(KEFIR_PARSER_RULE_REDUCE(mem, parser, constant, result));
    } else if (KEFIR_PARSER_RULE_MATCH(parser, string_literal)) {
        REQUIRE_OK(KEFIR_PARSER_RULE_REDUCE(mem, parser, string_literal, result));
    } else if (KEFIR_PARSER_RULE_MATCH(parser, generic_selection)) {
        REQUIRE_OK(KEFIR_PARSER_RULE_REDUCE(mem, parser, generic_selection, result));
    } else {
        REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
        REQUIRE_OK(kefir_parser_shift(mem, parser));
        REQUIRE_OK(KEFIR_PARSER_RULE_REDUCE(mem, parser, primary_expression,
                                            result));  // TODO Replace by assignment expression
        REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
        REQUIRE_OK(kefir_parser_shift(mem, parser));
    }
    return KEFIR_OK;
}
