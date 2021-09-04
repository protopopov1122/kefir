#include "kefir/core/basic-types.h"

kefir_result_category_t kefir_result_get_category(kefir_result_t res) {
    switch (res) {
        case KEFIR_OK:
        case KEFIR_YIELD:
            return KEFIR_RESULT_CATEGORY_NORMAL;

        case KEFIR_ITERATOR_END:
        case KEFIR_NO_MATCH:
        case KEFIR_NOT_FOUND:
        case KEFIR_ALREADY_EXISTS:
            return KEFIR_RESULT_CATEGORY_WARNING;

        case KEFIR_UNKNOWN_ERROR:
        case KEFIR_INTERNAL_ERROR:
        case KEFIR_OS_ERROR:
        case KEFIR_INVALID_PARAMETER:
        case KEFIR_INVALID_CHANGE:
        case KEFIR_INVALID_REQUEST:
        case KEFIR_INVALID_STATE:
        case KEFIR_OUT_OF_BOUNDS:
        case KEFIR_OBJALLOC_FAILURE:
        case KEFIR_MEMALLOC_FAILURE:
        case KEFIR_NOT_SUPPORTED:
        case KEFIR_NOT_IMPLEMENTED:
        case KEFIR_OUT_OF_SPACE:
            return KEFIR_RESULT_CATEGORY_GENERIC_ERROR;

        case KEFIR_LEXER_ERROR:
        case KEFIR_SYNTAX_ERROR:
        case KEFIR_ANALYSIS_ERROR:
        case KEFIR_NOT_CONSTANT:
        case KEFIR_STATIC_ASSERT:
            return KEFIR_RESULT_CATEGORY_LANGUAGE_ERROR;
    }
    return KEFIR_RESULT_CATEGORY_UNKNOWN;
}
