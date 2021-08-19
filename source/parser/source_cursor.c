#include "kefir/parser/source_cursor.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_lexer_source_cursor_init(struct kefir_lexer_source_cursor *cursor, const char *content,
                                              kefir_size_t length) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer source cursor"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid content"));

    *cursor = (struct kefir_lexer_source_cursor){0};
    cursor->content = content;
    cursor->length = length;
    return KEFIR_OK;
}

kefir_char32_t kefir_lexer_source_cursor_at(struct kefir_lexer_source_cursor *cursor, kefir_size_t count) {
    REQUIRE(cursor != NULL, U'\0');

    kefir_char32_t character = U'\0';
    kefir_size_t index = cursor->index;
    mbstate_t mbstate = cursor->mbstate;
    do {
        size_t rc = mbrtoc32(&character, cursor->content + index, cursor->length - index, &mbstate);
        switch (rc) {
            case 0:
            case (size_t) -1:
            case (size_t) -2:
            case (size_t) -3:
                character = U'\0';
                count = 0;
                break;

            default:
                index += rc;
                break;
        }
    } while (count--);
    return character;
}

kefir_result_t kefir_lexer_source_cursor_next(struct kefir_lexer_source_cursor *cursor, kefir_size_t count) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer source cursor"));
    size_t rc = mbrtoc32(NULL, cursor->content + cursor->index, cursor->length - cursor->index, &cursor->mbstate);
    while (count--) {
        switch (rc) {
            case (size_t) -1:
            case (size_t) -2:
            case (size_t) -3:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Invalid lexer source cursor input");

            case 0:
                return KEFIR_OK;

            default:
                cursor->index += rc;
                break;
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_source_cursor_save(const struct kefir_lexer_source_cursor *cursor,
                                              struct kefir_lexer_source_cursor_state *state) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer source cursor"));
    REQUIRE(state != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer lexer source cursor state"));

    state->index = cursor->index;
    state->mbstate = cursor->mbstate;
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_source_cursor_restore(struct kefir_lexer_source_cursor *cursor,
                                                 const struct kefir_lexer_source_cursor_state *state) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer source cursor"));
    REQUIRE(state != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer lexer source cursor state"));

    cursor->index = state->index;
    cursor->mbstate = state->mbstate;
    return KEFIR_OK;
}

const char *kefir_lexer_source_cursor_begin(struct kefir_lexer_source_cursor *cursor) {
    REQUIRE(cursor != NULL, NULL);
    return cursor->content;
}

const char *kefir_lexer_source_cursor_current(struct kefir_lexer_source_cursor *cursor) {
    REQUIRE(cursor != NULL, NULL);
    return cursor->content + cursor->index;
}

const char *kefir_lexer_source_cursor_end(struct kefir_lexer_source_cursor *cursor) {
    REQUIRE(cursor != NULL, NULL);
    return cursor->content + cursor->length;
}
