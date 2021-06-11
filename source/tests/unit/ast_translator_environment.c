#include "kefir/test/unit_test.h"
#include "kefir/test/util.h"
#include "kefir/ast/type.h"

#define ASSERT_DESIGNATOR_OFFSET(_mem, _env, _type, _designator, _size, _alignment, _offset)                  \
    do {                                                                                                      \
        struct kefir_ast_designator *designator = (_designator);                                              \
        struct kefir_ast_target_environment_object_info type_info;                                            \
        ASSERT_OK(KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO((_mem), (_env), (_type), designator, &type_info)); \
        if (designator != NULL) {                                                                             \
            ASSERT_OK(kefir_ast_designator_free((_mem), designator));                                         \
        }                                                                                                     \
        ASSERT(type_info.size == (_size));                                                                    \
        ASSERT(type_info.alignment == (_alignment));                                                          \
        ASSERT(type_info.relative_offset == (_offset));                                                       \
    } while (0)

DEFINE_CASE(ast_translator_environment1, "AST translator - environment object info") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_translator_environment env;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_struct_type *struct1_type = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, &type_bundle, "", &struct1_type);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct1_type, "x", kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct1_type, "y", kefir_ast_type_float(), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, struct1_type, "z",
                                    kefir_ast_type_array(&kft_mem, &type_bundle, kefir_ast_type_signed_short(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 16), NULL),
                                    NULL));

    struct kefir_ast_struct_type *struct2_type = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, &type_bundle, "", &struct2_type);
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, struct2_type, "field1", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, struct2_type, "field2", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct2_type, NULL, type1, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct2_type, "field3", type1, NULL));

    kefir_ast_target_environment_opaque_type_t opaque_type;
    ASSERT_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(&kft_mem, &env.target_env, type2, &opaque_type));

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, opaque_type, NULL, 96, 8, 0);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, opaque_type,
                             kefir_ast_new_member_desginator(&kft_mem, &symbols, "field1", NULL), 8, 8, 0);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, opaque_type,
                             kefir_ast_new_member_desginator(&kft_mem, &symbols, "field2", NULL), 4, 4, 8);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, opaque_type,
                             kefir_ast_new_member_desginator(&kft_mem, &symbols, "x", NULL), 1, 1, 12);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, opaque_type,
                             kefir_ast_new_member_desginator(&kft_mem, &symbols, "y", NULL), 4, 4, 16);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, opaque_type,
                             kefir_ast_new_member_desginator(&kft_mem, &symbols, "z", NULL), 32, 2, 20);

    for (kefir_size_t i = 0; i < 16; i++) {
        ASSERT_DESIGNATOR_OFFSET(
            &kft_mem, &env.target_env, opaque_type,
            kefir_ast_new_index_desginator(&kft_mem, i, kefir_ast_new_member_desginator(&kft_mem, &symbols, "z", NULL)),
            2, 2, 20 + i * 2);
    }

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, opaque_type,
                             kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL), 40, 4, 52);

    ASSERT_DESIGNATOR_OFFSET(
        &kft_mem, &env.target_env, opaque_type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "x",
                                        kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL)),
        1, 1, 52);

    ASSERT_DESIGNATOR_OFFSET(
        &kft_mem, &env.target_env, opaque_type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "y",
                                        kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL)),
        4, 4, 56);

    ASSERT_DESIGNATOR_OFFSET(
        &kft_mem, &env.target_env, opaque_type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "z",
                                        kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL)),
        32, 2, 60);

    for (kefir_size_t i = 0; i < 16; i++) {
        ASSERT_DESIGNATOR_OFFSET(
            &kft_mem, &env.target_env, opaque_type,
            kefir_ast_new_index_desginator(
                &kft_mem, i,
                kefir_ast_new_member_desginator(&kft_mem, &symbols, "z",
                                                kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL))),
            2, 2, 60 + i * 2);
    }

    ASSERT_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(&kft_mem, &env.target_env, opaque_type));

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

#undef ASSERT_DESIGNATOR_OFFSET

#define ASSERT_OBJECT_OFFSET(_mem, _env, _type, _min, _max, _size)                                          \
    do {                                                                                                    \
        kefir_ast_target_environment_opaque_type_t opaque_type;                                             \
        ASSERT_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE((_mem), (_env), (_type), &opaque_type));            \
        for (kefir_int64_t i = (_min); i < (_max); i++) {                                                   \
            kefir_int64_t offset;                                                                           \
            ASSERT_OK(KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_OFFSET((_mem), (_env), opaque_type, i, &offset)); \
            ASSERT(i *(_size) == offset);                                                                   \
        }                                                                                                   \
        ASSERT_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE((_mem), (_env), opaque_type));                     \
    } while (0)

DEFINE_CASE(ast_translator_environment2, "AST translator - environment object offset") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_translator_environment env;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_char(), -100, 100, 1);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_unsigned_char(), -100, 100, 1);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_signed_char(), -100, 100, 1);

    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_signed_short(), -100, 100, 2);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_unsigned_short(), -100, 100, 2);

    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_signed_int(), -100, 100, 4);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_unsigned_int(), -100, 100, 4);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_float(), -100, 100, 4);

    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_signed_long(), -100, 100, 8);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_unsigned_long(), -100, 100, 8);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_signed_long_long(), -100, 100, 8);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_unsigned_long_long(), -100, 100, 8);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, kefir_ast_type_double(), -100, 100, 8);

    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env,
                         kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void()), -100, 100, 8);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env,
                         kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_char()), -100, 100, 8);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env,
                         kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_int()), -100, 100, 8);
    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env,
                         kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_double()), -100, 100, 8);
    ASSERT_OBJECT_OFFSET(
        &kft_mem, &env.target_env,
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_short())),
        -100, 100, 8);

    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env,
                         kefir_ast_type_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(),
                                              kefir_ast_constant_expression_integer(&kft_mem, 16), NULL),
                         -100, 100, 16);

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, &type_bundle, "", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "x", kefir_ast_type_unsigned_long(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "y",
                                          kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_bool()), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "z",
                                    kefir_ast_type_array(&kft_mem, &type_bundle, kefir_ast_type_float(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 5), NULL),
                                    NULL));

    ASSERT_OBJECT_OFFSET(&kft_mem, &env.target_env, type1, -100, 100, 40);

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

#undef ASSERT_OBJECT_OFFSET
