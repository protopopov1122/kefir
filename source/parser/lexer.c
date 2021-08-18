#include "kefir/parser/lexer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_lexer_init(struct kefir_lexer *lexer, struct kefir_symbol_table *symbols,
                                struct kefir_lexer_source_cursor *cursor) {
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer source cursor"));

    lexer->symbols = symbols;
    lexer->cursor = cursor;
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_apply(struct kefir_mem *mem, struct kefir_lexer *lexer, kefir_lexer_callback_fn_t callback,
                                 void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(callback != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid callback"));

    struct kefir_lexer_source_cursor_state state;
    REQUIRE_OK(kefir_lexer_source_cursor_save(lexer->cursor, &state));
    kefir_result_t res = callback(mem, lexer, payload);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_lexer_source_cursor_restore(lexer->cursor, &state));
    }
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_cursor_match_string(struct kefir_lexer_source_cursor *cursor, const char32_t *string) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer source cursor"));
    REQUIRE(string != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string"));

    for (kefir_size_t index = 0; string[index] != '\0'; index++) {
        if (kefir_lexer_source_cursor_at(cursor, index) != string[index]) {
            return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match provided string");
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_next(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));

    REQUIRE_OK(kefir_lexer_match_keyword(mem, lexer, token));
    return KEFIR_OK;
}
