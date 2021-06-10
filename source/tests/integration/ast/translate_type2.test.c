#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ir/format.h"
#include "kefir/test/util.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ir_type ir_type;
    struct kefir_irbuilder_type builder;
    struct kefir_ast_translator_environment env;

    REQUIRE_OK(kefir_ir_type_alloc(mem, 0, &ir_type));
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, &ir_type));
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    for (unsigned int i = 0; i <= 4; i++) {
        unsigned int alignment = 1 << i;
        REQUIRE_OK(kefir_ast_translate_object_type(mem, kefir_ast_type_void(), alignment, &env, &builder, NULL));
        REQUIRE_OK(kefir_ast_translate_object_type(mem, kefir_ast_type_bool(), alignment, &env, &builder, NULL));
        REQUIRE_OK(
            kefir_ast_translate_object_type(mem, kefir_ast_type_unsigned_char(), alignment, &env, &builder, NULL));
        REQUIRE_OK(kefir_ast_translate_object_type(mem, kefir_ast_type_signed_char(), alignment, &env, &builder, NULL));
        REQUIRE_OK(
            kefir_ast_translate_object_type(mem, kefir_ast_type_unsigned_short(), alignment, &env, &builder, NULL));
        REQUIRE_OK(
            kefir_ast_translate_object_type(mem, kefir_ast_type_signed_short(), alignment, &env, &builder, NULL));
        REQUIRE_OK(
            kefir_ast_translate_object_type(mem, kefir_ast_type_unsigned_int(), alignment, &env, &builder, NULL));
        REQUIRE_OK(kefir_ast_translate_object_type(mem, kefir_ast_type_signed_int(), alignment, &env, &builder, NULL));
        REQUIRE_OK(
            kefir_ast_translate_object_type(mem, kefir_ast_type_unsigned_long(), alignment, &env, &builder, NULL));
        REQUIRE_OK(kefir_ast_translate_object_type(mem, kefir_ast_type_signed_long(), alignment, &env, &builder, NULL));
        REQUIRE_OK(
            kefir_ast_translate_object_type(mem, kefir_ast_type_unsigned_long_long(), alignment, &env, &builder, NULL));
        REQUIRE_OK(
            kefir_ast_translate_object_type(mem, kefir_ast_type_signed_long_long(), alignment, &env, &builder, NULL));
        REQUIRE_OK(kefir_ast_translate_object_type(mem, kefir_ast_type_float(), alignment, &env, &builder, NULL));
        REQUIRE_OK(kefir_ast_translate_object_type(mem, kefir_ast_type_double(), alignment, &env, &builder, NULL));
    }
    REQUIRE_OK(kefir_ir_format_type(stdout, &ir_type));

    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    REQUIRE_OK(kefir_ir_type_free(mem, &ir_type));
    return KEFIR_OK;
}
