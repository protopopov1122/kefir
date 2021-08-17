#ifndef KEFIR_PARSER_SOURCE_CURSOR_H_
#define KEFIR_PARSER_SOURCE_CURSOR_H_

#include "kefir/parser/base.h"
#include "kefir/core/basic-types.h"
#include <uchar.h>

typedef struct kefir_lexer_source_cursor {
    const char *content;
    kefir_size_t index;
    kefir_size_t length;
    mbstate_t mbstate;
} kefir_lexer_source_cursor_t;

typedef struct kefir_lexer_source_cursor_state {
    kefir_size_t index;
    mbstate_t mbstate;
} kefir_lexer_source_cursor_state_t;

kefir_result_t kefir_lexer_source_cursor_init(struct kefir_lexer_source_cursor *, const char *, kefir_size_t);
char32_t kefir_lexer_source_cursor_at(struct kefir_lexer_source_cursor *, kefir_size_t);
kefir_result_t kefir_lexer_source_cursor_next(struct kefir_lexer_source_cursor *, kefir_size_t);
kefir_result_t kefir_lexer_source_cursor_save(const struct kefir_lexer_source_cursor *,
                                              struct kefir_lexer_source_cursor_state *);
kefir_result_t kefir_lexer_source_cursor_restore(struct kefir_lexer_source_cursor *,
                                                 const struct kefir_lexer_source_cursor_state *);
const char *kefir_lexer_source_cursor_begin(struct kefir_lexer_source_cursor *);
const char *kefir_lexer_source_cursor_current(struct kefir_lexer_source_cursor *);
const char *kefir_lexer_source_cursor_end(struct kefir_lexer_source_cursor *);

#endif