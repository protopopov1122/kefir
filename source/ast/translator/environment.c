#include "kefir/ast/translator/environment.h"
#include "kefir/ast/translator/translator.h"
#include "kefir/ir/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_environment_init(struct kefir_ast_translator_environment *env,
                                                 struct kefir_ir_target_platform *target_platform) {    
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST translator environment"));
    REQUIRE(target_platform != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid IR target platform"));
    env->target_platform = target_platform;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_environment_target_stored_type_info(struct kefir_mem *mem,
                                                                    const struct kefir_ast_translator_environment *env,
                                                                    const struct kefir_ast_type *type,
                                                                    struct kefir_ir_target_type_info *type_info) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST translator environment"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(type_info != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR target type info"));

    struct kefir_ir_type ir_type;
    struct kefir_irbuilder_type builder;
    REQUIRE_OK(kefir_ir_type_alloc(mem, 0, &ir_type));
    kefir_result_t res = kefir_irbuilder_type_init(mem, &builder, &ir_type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &ir_type);
        return res;
    });
    res = kefir_ast_translate_stored_object_type(type, &builder, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&builder);
        kefir_ir_type_free(mem, &ir_type);
        return res;
    });
    res = KEFIR_IRBUILDER_TYPE_FREE(&builder);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &ir_type);
        return res;
    });
    res = KEFIR_IR_TARGET_PLATFORM_TYPE_INFO(env->target_platform, mem, &ir_type, type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &ir_type);
        return res;
    });
    REQUIRE_OK(kefir_ir_type_free(mem, &ir_type));
    return KEFIR_OK;
}
