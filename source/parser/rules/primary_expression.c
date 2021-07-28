#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_parser_apply_rule_primary_expression(struct kefir_mem *mem, struct kefir_parser *parser,
                                                          struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    kefir_result_t res = KEFIR_PARSER_RULE_APPLY(mem, parser, identifier, result);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = KEFIR_PARSER_RULE_APPLY(mem, parser, constant, result);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = KEFIR_PARSER_RULE_APPLY(mem, parser, string_literal, result);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = KEFIR_PARSER_RULE_APPLY(mem, parser, generic_selection, result);
    REQUIRE(res == KEFIR_NO_MATCH, res);

    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, primary_expression,
                                       result));  // TODO Replace by assignment expression
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}
