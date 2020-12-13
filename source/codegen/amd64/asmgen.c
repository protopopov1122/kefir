#include <stdarg.h>
#include "kefir/codegen/amd64/asmgen.h"
#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t amd64_newline(struct kefir_amd64_asmgen *asmgen, unsigned int count) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    while (count--) {
        fprintf(out, "\n");
    }
    return KEFIR_OK;
}

static kefir_result_t amd64_comment(struct kefir_amd64_asmgen *asmgen, const char *identifier) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    fprintf(out, "; %s\n", identifier);
    return KEFIR_OK;
}

static kefir_result_t amd64_label(struct kefir_amd64_asmgen *asmgen, const char *identifier) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    fprintf(out, "%s:\n", identifier);
    return KEFIR_OK;
}

static kefir_result_t amd64_global(struct kefir_amd64_asmgen *asmgen, const char *identifier) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    fprintf(out, "global %s\n", identifier);
    return KEFIR_OK;
}

static kefir_result_t amd64_external(struct kefir_amd64_asmgen *asmgen, const char *identifier) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    fprintf(out, "extern %s\n", identifier);
    return KEFIR_OK;
}

static kefir_result_t amd64_section(struct kefir_amd64_asmgen *asmgen, const char *identifier) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    fprintf(out, "SECTION %s\n", identifier);
    return KEFIR_OK;
}

static kefir_result_t amd64_instr(struct kefir_amd64_asmgen *asmgen, const char *opcode, unsigned int argc, ...) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    va_list(args);
    va_start(args, argc);
    fprintf(out, "    %s", opcode);
    while (argc--) {
        const char *param = va_arg(args, const char *);
        fprintf(out, " %s", param);
        if (argc > 0) {
            fprintf(out, ",");
        }
    }
    fprintf(out, "\n");
    va_end(args);
    return KEFIR_OK;
}

static kefir_result_t amd64_rawdata(struct kefir_amd64_asmgen *asmgen, kefir_amd64_asmgen_datawidth_t width, unsigned int argc, ...) {
    REQUIRE(asmgen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 assembly generator"));
    FILE *out = (FILE *) asmgen->data;
    REQUIRE(out != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid output file for AMD64 assembly"));
    va_list(args);
    va_start(args, argc);
    fprintf(out, "    d%c", (char) width);
    while (argc--) {
        const char *param = va_arg(args, const char *);
        fprintf(out, " %s", param);
        if (argc > 0) {
            fprintf(out, ",");
        }
    }
    fprintf(out, "\n");
    va_end(args);
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
    asmgen->data = (void *) out;
    return KEFIR_OK;
}