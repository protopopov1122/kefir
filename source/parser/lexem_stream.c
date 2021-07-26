#include "kefir/parser/lexem_stream.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t token_array_stream_next(struct kefir_mem *mem, struct kefir_token_stream *stream,
                                              struct kefir_token *dst) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(stream != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token stream"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    ASSIGN_DECL_CAST(struct kefir_token_array_stream *, array_stream, stream->payload);

    REQUIRE(array_stream->index < array_stream->length,
            KEFIR_SET_ERROR(KEFIR_ITERATOR_END, "Token array stream is empty"));
    REQUIRE_OK(kefir_token_copy(mem, dst, &array_stream->tokens[array_stream->index]));
    array_stream->index++;
    return KEFIR_OK;
}

static kefir_result_t token_array_stream_free(struct kefir_mem *mem, struct kefir_token_stream *stream) {
    UNUSED(mem);
    REQUIRE(stream != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token stream"));
    ASSIGN_DECL_CAST(struct kefir_token_array_stream *, array_stream, stream->payload);
    array_stream->stream.next = NULL;
    array_stream->stream.free = NULL;
    array_stream->stream.payload = NULL;
    array_stream->tokens = NULL;
    array_stream->length = 0;
    array_stream->index = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_token_array_stream_init(struct kefir_token_array_stream *stream, const struct kefir_token *tokens,
                                             kefir_size_t length) {
    REQUIRE(stream != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token array stream"));
    REQUIRE(tokens != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token array"));

    stream->tokens = tokens;
    stream->index = 0;
    stream->length = length;

    stream->stream.next = token_array_stream_next;
    stream->stream.free = token_array_stream_free;
    stream->stream.payload = stream;
    return KEFIR_OK;
}
