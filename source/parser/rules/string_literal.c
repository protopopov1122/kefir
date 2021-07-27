#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_bool_t kefir_parser_match_rule_string_literal(const struct kefir_parser *parser) {
    REQUIRE(parser != NULL, false);
    return PARSER_TOKEN_IS_STRING_LITERAL(parser, 0);
}

kefir_result_t kefir_parser_reduce_rule_string_literal(struct kefir_mem *mem, struct kefir_parser *parser,
                                                       struct kefir_ast_node_base **result) {
    REDUCE_PROLOGUE(mem, parser, result, string_literal);
    const struct kefir_token *token = PARSER_LOOKAHEAD(parser, 0);
    REQUIRE_ALLOC(result,
                  KEFIR_AST_NODE_BASE(
                      kefir_ast_new_string_literal(mem, token->string_literal.content, token->string_literal.length)),
                  "Failed to allocate AST string literal");
    REQUIRE_OK(kefir_parser_shift(mem, parser));
    return KEFIR_OK;
}
