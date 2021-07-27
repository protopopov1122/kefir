#include "kefir/parser/parser.h"
#include "kefir/parser/rules.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_parser_init(struct kefir_mem *mem, struct kefir_parser *parser, struct kefir_symbol_table *symbols,
                                 struct kefir_token_stream *stream) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(stream != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token stream"));

    parser->symbols = symbols;
    parser->tokens = stream;
    kefir_result_t res = KEFIR_OK;
    kefir_size_t i = 0;
    for (; res == KEFIR_OK && i < KEFIR_PARSER_LOOKAHEAD; i++) {
        res = parser->tokens->next(mem, parser->tokens, &parser->lookahead[i]);
    }

    REQUIRE_ELSE(res != KEFIR_OK, {
        for (kefir_size_t j = 0; j < i; j++) {
            kefir_token_free(mem, &parser->lookahead[j]);
        }
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_free(struct kefir_mem *mem, struct kefir_parser *parser) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    for (kefir_size_t i = 0; i < KEFIR_PARSER_LOOKAHEAD; i++) {
        REQUIRE_OK(kefir_token_free(mem, &parser->lookahead[i]));
    }
    parser->tokens = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_shift(struct kefir_mem *mem, struct kefir_parser *parser) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE_OK(kefir_token_free(mem, &parser->lookahead[0]));
    if (KEFIR_PARSER_LOOKAHEAD > 1) {
        for (kefir_int_t i = 0; i < KEFIR_PARSER_LOOKAHEAD - 1; i++) {
            REQUIRE_OK(kefir_token_move(&parser->lookahead[i], &parser->lookahead[i + 1]));
        }
    }
    kefir_result_t res = parser->tokens->next(mem, parser->tokens, &parser->lookahead[KEFIR_PARSER_LOOKAHEAD - 1]);
    if (res == KEFIR_ITERATOR_END) {
        REQUIRE_OK(kefir_token_new_sentinel(&parser->lookahead[KEFIR_PARSER_LOOKAHEAD - 1]));
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_parser_next(struct kefir_mem *mem, struct kefir_parser *parser,
                                 struct kefir_ast_node_base **result) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST node"));

    REQUIRE(KEFIR_PARSER_RULE_MATCH(parser, primary_expression),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match primary expression"));
    REQUIRE_OK(KEFIR_PARSER_RULE_REDUCE(mem, parser, primary_expression, result));
    return KEFIR_OK;
}
