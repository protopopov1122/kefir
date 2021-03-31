#include "kefir/test/unit_test.h"
#include "kefir/test/util.h"
#include "kefir/ast/type.h"

#define ASSERT_DESIGNATOR_OFFSET(_mem, _env, _type, _designator, _offset) \
    do { \
        kefir_size_t offset = 0; \
        struct kefir_ast_designator *designator = (_designator); \
        ASSERT_OK(KEFIR_AST_TARGET_ENVIRONMENT_OFFSET_OF((_mem), (_env), (_type), designator, &offset)); \
        if (designator != NULL) { \
            ASSERT_OK(kefir_ast_designator_free((_mem), designator)); \
        } \
        ASSERT(offset == (_offset)); \
    } while (0)

DEFINE_CASE(ast_translator_environment1, "AST translator - environment #1")
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_translator_environment env;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_struct_type *struct1_type = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, &type_bundle,
        "", &struct1_type);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct1_type,
        "x", kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct1_type,
        "y", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct1_type,
        "z", kefir_ast_type_array(&kft_mem, &type_bundle,
            kefir_ast_type_signed_short(), kefir_ast_constant_expression_integer(&kft_mem, 16), NULL), NULL));

    struct kefir_ast_struct_type *struct2_type = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, &type_bundle,
        "", &struct2_type);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct2_type,
        "field1", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct2_type,
        "field2", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct2_type,
        NULL, type1, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct2_type,
        "field3", type1, NULL));

    struct kefir_ast_target_type_info type2_info;
    ASSERT_OK(KEFIR_AST_TARGET_ENVIRONMENT_TYPE_INFO(&kft_mem, &env.target_env, type2, &type2_info));

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        NULL, 0);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "field1", NULL),
        0);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "field2", NULL),
        8);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "x", NULL),
        12);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "y", NULL),
        16);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "z", NULL),
        20);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_index_desginator(&kft_mem, 0,
            kefir_ast_new_member_desginator(&kft_mem, &symbols, "z", NULL)),
        20);

    for (kefir_size_t i = 0; i < 16; i++) {
        ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
            kefir_ast_new_index_desginator(&kft_mem, i,
                kefir_ast_new_member_desginator(&kft_mem, &symbols, "z", NULL)),
            20 + i * 2);
    }

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL),
        52);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "x",
            kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL)),
        52);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "y",
            kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL)),
        56);

    ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
        kefir_ast_new_member_desginator(&kft_mem, &symbols, "z",
            kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL)),
        60);

    for (kefir_size_t i = 0; i < 16; i++) {
        ASSERT_DESIGNATOR_OFFSET(&kft_mem, &env.target_env, type2_info.type,
            kefir_ast_new_index_desginator(&kft_mem, i,
                kefir_ast_new_member_desginator(&kft_mem, &symbols, "z", 
                    kefir_ast_new_member_desginator(&kft_mem, &symbols, "field3", NULL))),
            60 + i * 2);
    }

    ASSERT_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE_INFO(&kft_mem, &env.target_env, &type2_info));

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE