#include <stdarg.h>
#include <string.h>
#include <uchar.h>
#include "kefir/codegen/amd64/asmgen.h"
#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

#define INDENTATION "    "

static kefir_result_t amd64_newline(struct kefir_amd64_asmgen *asmgen, unsigned int count) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    while (count--) {
        fprintf(out, "\n");
    }
    return KEFIR_OK;
}

static kefir_result_t amd64_comment(struct kefir_amd64_asmgen *asmgen, const char *format, ...) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    if (asmgen->settings.enable_comments) {
        FILE *out = (FILE *) asmgen->data;
        REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
        if (!asmgen->state.empty) {
            fprintf(out, "\n");
        }
        asmgen->state.empty = false;
        fprintf(out, "; ");
        va_list args;
        va_start(args, format);
        vfprintf(out, format, args);
        va_end(args);
    }
    return KEFIR_OK;
}

static kefir_result_t amd64_label(struct kefir_amd64_asmgen *asmgen, const char *format, ...) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    if (!asmgen->state.empty) {
        fprintf(out, "\n");
    }
    asmgen->state.empty = false;
    va_list args;
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    fprintf(out, ":");
    return KEFIR_OK;
}

static kefir_result_t amd64_global(struct kefir_amd64_asmgen *asmgen, const char *format, ...) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    if (!asmgen->state.empty) {
        fprintf(out, "\n");
    }
    asmgen->state.empty = false;
    fprintf(out, "global ");
    va_list args;
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    return KEFIR_OK;
}

static kefir_result_t amd64_external(struct kefir_amd64_asmgen *asmgen, const char *identifier) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    if (!asmgen->state.empty) {
        fprintf(out, "\n");
    }
    asmgen->state.empty = false;
    fprintf(out, "extern %s", identifier);
    return KEFIR_OK;
}

static kefir_result_t amd64_section(struct kefir_amd64_asmgen *asmgen, const char *identifier) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    if (!asmgen->state.empty) {
        fprintf(out, "\n");
    }
    asmgen->state.empty = false;
    fprintf(out, "SECTION %s", identifier);
    return KEFIR_OK;
}

static kefir_result_t amd64_instr(struct kefir_amd64_asmgen *asmgen, const char *opcode) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    if (!asmgen->state.empty) {
        fprintf(out, "\n");
    }
    asmgen->state.empty = false;
    if (asmgen->settings.enable_identation) {
        fprintf(out, "%s", INDENTATION);
    }
    fprintf(out, "%s", opcode);
    asmgen->state.arguments = 0;
    return KEFIR_OK;
}

static kefir_result_t amd64_rawdata(struct kefir_amd64_asmgen *asmgen, kefir_amd64_asmgen_datawidth_t width) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    if (!asmgen->state.empty) {
        fprintf(out, "\n");
    }
    asmgen->state.empty = false;
    if (asmgen->settings.enable_identation) {
        fprintf(out, "%s", INDENTATION);
    }
    fprintf(out, "d%c", (char) width);
    asmgen->state.arguments = 0;
    return KEFIR_OK;
}

static kefir_result_t amd64_multrawdata(struct kefir_amd64_asmgen *asmgen, kefir_size_t times, kefir_amd64_asmgen_datawidth_t width) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    if (!asmgen->state.empty) {
        fprintf(out, "\n");
    }
    asmgen->state.empty = false;
    if (asmgen->settings.enable_identation) {
        fprintf(out, "%s", INDENTATION);
    }
    fprintf(out, "times " KEFIR_SIZE_FMT " d%c", times, (char) width);
    asmgen->state.arguments = 0;
    return KEFIR_OK;
}

static kefir_result_t amd64_argument(struct kefir_amd64_asmgen *asmgen, const char *format, ...) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    if (asmgen->state.arguments > 0) {
        fprintf(out, ", ");
    } else {
        fprintf(out, " ");
    }
    asmgen->state.arguments++;
    va_list args;
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    return KEFIR_OK;
}

kefir_result_t amd64_string_literal(struct kefir_amd64_asmgen *asmgen, const char *literal, kefir_size_t length) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    REQUIRE(literal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string literal"));

    FILE *out = (FILE *) asmgen->data;
    if (asmgen->state.arguments > 0) {
        fprintf(out, ", ");
    } else {
        fprintf(out, " ");
    }
    asmgen->state.arguments++;
    fprintf(out, "`");

    mbstate_t state;
    const char *end = literal + length;
    size_t sz = 0;
    char32_t wide_char;
    while (literal < end && (*literal == '\0' || (sz = mbrtoc32(&wide_char, literal, end - literal, &state)) != 0)) {
        if (*literal == '\0') {
            fprintf(out, "\\000");
            literal++;
            continue;
        }
        switch (wide_char) {
            case U'\'':
                fprintf(out, "\\\'");
                break;

            case U'\"':
                fprintf(out, "\\\"");
                break;

            case U'`':
                fprintf(out, "\\`");
                break;

            case U'\\':
                fprintf(out, "\\\\");
                break;

            case U'\?':
                fprintf(out, "\\?");
                break;

            case U'\a':
                fprintf(out, "\\a");
                break;

            case U'\b':
                fprintf(out, "\\b");
                break;

            case U'\t':
                fprintf(out, "\\t");
                break;

            case U'\n':
                fprintf(out, "\\n");
                break;

            case U'\v':
                fprintf(out, "\\v");
                break;

            case U'\f':
                fprintf(out, "\\f");
                break;

            case U'\r':
                fprintf(out, "\\r");
                break;

            default:
                fwrite(literal, 1, sz, out);
                break;
        }
        literal += sz;
    }
    fprintf(out, "`");
    return KEFIR_OK;
}

static kefir_result_t amd64_close(struct kefir_amd64_asmgen *asmgen) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    fclose(out);
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_nasm_gen_init(struct kefir_amd64_asmgen *asmgen, FILE *out) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator pointer"));
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    asmgen->newline = amd64_newline;
    asmgen->comment = amd64_comment;
    asmgen->label = amd64_label;
    asmgen->global = amd64_global;
    asmgen->external = amd64_external;
    asmgen->section = amd64_section;
    asmgen->instr = amd64_instr;
    asmgen->close = amd64_close;
    asmgen->rawdata = amd64_rawdata;
    asmgen->string_literal = amd64_string_literal;
    asmgen->mulrawdata = amd64_multrawdata;
    asmgen->argument = amd64_argument;
    asmgen->data = (void *) out;
    asmgen->state.empty = true;
    asmgen->state.arguments = 0;
    asmgen->settings.enable_comments = true;
    asmgen->settings.enable_identation = true;
    return KEFIR_OK;
}