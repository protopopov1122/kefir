#ifndef KEFIR_PARSER_LEXER_H_
#define KEFIR_PARSER_LEXER_H_

#include "kefir/parser/lexem.h"
#include "kefir/parser/source_cursor.h"
#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"
#include "kefir/core/trie.h"
#include "kefir/parser/context.h"

typedef struct kefir_lexer {
    struct kefir_symbol_table *symbols;
    struct kefir_lexer_source_cursor *cursor;
    const struct kefir_parser_integral_types *integral_types;

    struct kefir_trie punctuators;
    struct kefir_trie keywords;
} kefir_lexer_t;

typedef kefir_result_t (*kefir_lexer_callback_fn_t)(struct kefir_mem *, struct kefir_lexer *, void *);

kefir_result_t kefir_lexer_init(struct kefir_mem *, struct kefir_lexer *, struct kefir_symbol_table *,
                                struct kefir_lexer_source_cursor *, const struct kefir_parser_integral_types *);
kefir_result_t kefir_lexer_free(struct kefir_mem *, struct kefir_lexer *);
kefir_result_t kefir_lexer_apply(struct kefir_mem *, struct kefir_lexer *, kefir_lexer_callback_fn_t, void *);
kefir_result_t kefir_lexer_next(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

kefir_result_t kefir_lexer_cursor_skip_whitespaces(struct kefir_lexer_source_cursor *);
kefir_result_t kefir_lexer_cursor_next_universal_character(struct kefir_lexer_source_cursor *, kefir_char32_t *);
kefir_result_t kefir_lexer_cursor_next_escape_sequence(struct kefir_lexer_source_cursor *, kefir_char32_t *);

kefir_result_t kefir_lexer_init_punctuators(struct kefir_mem *, struct kefir_lexer *);
kefir_result_t kefir_lexer_match_punctuator(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

kefir_result_t kefir_lexer_init_keywords(struct kefir_mem *, struct kefir_lexer *);
kefir_result_t kefir_lexer_get_keyword(struct kefir_lexer *, const kefir_char32_t *, kefir_keyword_token_t *);
kefir_result_t kefir_lexer_match_identifier_or_keyword(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

kefir_result_t kefir_lexer_next_narrow_string_literal(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_match_string_literal(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

kefir_result_t kefir_lexer_match_floating_constant(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_match_integer_constant(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_match_character_constant(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_match_constant(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

#endif
