#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

DEFINE_MATCH(STRING_LITERAL) {
    REQUIRE(PARSER_TOKEN_IS_STRING_LITERAL(parser, 0), KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match string literal"));
}
END_PROCEDURE

DEFINE_REDUCE(STRING_LITERAL) {
    const struct kefir_token *token = PARSER_LOOKAHEAD(parser, 0);
    ALLOC_NODE(result, kefir_ast_new_string_literal, mem, token->string_literal.content, token->string_literal.length);
    PARSER_SHIFT(1);
}
END_PROCEDURE

DEFINE_RULE(STRING_LITERAL);
