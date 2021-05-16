#include "kefir/test/util.h"
#include "kefir/codegen/amd64/system-v/platform.h"

static kefir_bool_t init_done = false;
static struct kefir_ir_target_platform IR_TARGET;
static struct kefir_ast_translator_environment TRANSLATOR_ENV;

struct kefir_ir_target_platform *kft_util_get_ir_target_platform() {
    if (!init_done) {
        REQUIRE(kefir_codegen_amd64_sysv_target_platform(&IR_TARGET) == KEFIR_OK, NULL);
        REQUIRE(kefir_ast_translator_environment_init(&TRANSLATOR_ENV, &IR_TARGET) == KEFIR_OK, NULL);
        init_done = true;
    }
    return &IR_TARGET;
}

struct kefir_ast_translator_environment *kft_util_get_translator_environment() {
    if (!init_done) {
        REQUIRE(kefir_codegen_amd64_sysv_target_platform(&IR_TARGET) == KEFIR_OK, NULL);
        REQUIRE(kefir_ast_translator_environment_init(&TRANSLATOR_ENV, &IR_TARGET) == KEFIR_OK, NULL);
        init_done = true;
    }
    return &TRANSLATOR_ENV;
}
