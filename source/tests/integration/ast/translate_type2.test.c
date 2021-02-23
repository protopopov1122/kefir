#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast/translator/translator.h"
#include "kefir/ir/format.h"

static kefir_result_t target_platform_type_info(struct kefir_mem *mem,
                                              struct kefir_ir_target_platform *target_platform,
                                              const struct kefir_ir_type *type,
                                              struct kefir_ir_target_type_info *type_info) {
    UNUSED(mem);
    UNUSED(target_platform);
    UNUSED(type);
    UNUSED(type_info);
    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected call to target platform type info");
}

static kefir_result_t target_platform_free(struct kefir_mem *mem,
                                         struct kefir_ir_target_platform *target_platform) {
    UNUSED(mem);
    UNUSED(target_platform);
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ir_type ir_type;
    struct kefir_irbuilder_type builder;
    struct kefir_ast_translator_environment env;
    struct kefir_ir_target_platform target_platform = {
        .type_info = target_platform_type_info,
        .free = target_platform_free,
        .payload = NULL
    };

    REQUIRE_OK(kefir_ir_type_alloc(mem, 0, &ir_type));
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, &ir_type));
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, &target_platform));

    for (unsigned int i = 0; i <= 4; i++) {
        unsigned int alignment = 1 << i;
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_void(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_bool(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_unsigned_char(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_signed_char(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_unsigned_short(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_signed_short(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_unsigned_int(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_signed_int(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_unsigned_long(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_signed_long(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_unsigned_long_long(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_signed_long_long(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_float(), alignment, &env, &builder));
        REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, kefir_ast_type_double(), alignment, &env, &builder));
    }
    REQUIRE_OK(kefir_ir_format_type(stdout, &ir_type));

    REQUIRE_OK(KEFIR_IR_TARGET_PLATFORM_FREE(&target_platform, mem));
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    REQUIRE_OK(kefir_ir_type_free(mem, &ir_type));
    return KEFIR_OK;
}