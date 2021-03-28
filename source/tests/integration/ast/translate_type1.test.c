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
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ir_target_platform target_platform = {
        .type_info = target_platform_type_info,
        .free = target_platform_free,
        .payload = NULL
    };

    REQUIRE_OK(kefir_ir_type_alloc(mem, 0, &ir_type));
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, &ir_type));
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, &target_platform));
    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *type1 = kefir_ast_type_array(mem, &type_bundle,
        kefir_ast_type_pointer(mem, &type_bundle, kefir_ast_type_void()),
            kefir_ast_constant_expression_integer(mem, 10), NULL);
    REQUIRE(type1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, type1, 0, &env, &builder));
    
    struct kefir_ast_struct_type *struct_type2;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(mem, &type_bundle, "struct1", &struct_type2);
    REQUIRE(type2 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ast_struct_type *union_type3;
    const struct kefir_ast_type *type3 = kefir_ast_type_union(mem, &type_bundle, "union1", &union_type3);
    REQUIRE(type3 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ast_function_type *function_type4 = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_function(mem, &type_bundle, kefir_ast_type_void(), "func1", &function_type4);

    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type2, "field1", kefir_ast_type_float(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, union_type3, "sub1", kefir_ast_type_array(
        mem, &type_bundle, kefir_ast_type_qualified(
            mem, &type_bundle, kefir_ast_type_unsigned_char(), (const struct kefir_ast_type_qualification){
                .constant = true,
                .restricted = false,
                .volatile_type = false
            }), kefir_ast_constant_expression_integer(mem, 256), NULL), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, union_type3, "sub2", kefir_ast_type_unsigned_long_long(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type2, NULL, type3, NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type2, NULL, kefir_ast_type_pointer(
        mem, &type_bundle, type4), NULL));
    REQUIRE_OK(kefir_ast_struct_type_bitfield(mem, &symbols, struct_type2, "h1", kefir_ast_type_signed_short(), NULL,
        kefir_ast_constant_expression_integer(mem, 6)));
    REQUIRE_OK(kefir_ast_struct_type_bitfield(mem, &symbols, struct_type2, "h2", kefir_ast_type_signed_short(), NULL,
        kefir_ast_constant_expression_integer(mem, 1)));
    REQUIRE_OK(kefir_ast_struct_type_bitfield(mem, &symbols, struct_type2, "h10", kefir_ast_type_signed_short(), NULL,
        kefir_ast_constant_expression_integer(mem, 0)));
    REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, type2, 0, &env, &builder));

    REQUIRE_OK(kefir_ir_format_type(stdout, &ir_type));
    REQUIRE_OK(kefir_ast_type_bundle_free(mem, &type_bundle));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    REQUIRE_OK(KEFIR_IR_TARGET_PLATFORM_FREE(&target_platform, mem));
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    REQUIRE_OK(kefir_ir_type_free(mem, &ir_type));
    return KEFIR_OK;
}