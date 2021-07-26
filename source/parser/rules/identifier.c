#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

DEFINE_MATCH(IDENTIFIER) {
    REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0), KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match identifier"));
}
END_PROCEDURE

DEFINE_REDUCE(IDENTIFIER) {
    ALLOC_NODE(result, kefir_ast_new_identifier, mem, parser->symbols, PARSER_LOOKAHEAD(parser, 0)->identifier);
    PARSER_SHIFT(1);
}
END_PROCEDURE

DEFINE_RULE(IDENTIFIER);
