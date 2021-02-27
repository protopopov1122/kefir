#include "kefir/test/unit_test.h"
#include "kefir/ast/type.h"


#define ASSERT_FUNCTION_PARAM_ADJ_TYPE(_func, _index, _type) \
    do { \
        const struct kefir_ast_function_type_parameter *_param = NULL; \
        ASSERT_OK(kefir_ast_type_function_get_parameter((_func), (_index), &_param)); \
        ASSERT(_param->type != NULL); \
        ASSERT(KEFIR_AST_TYPE_SAME(_param->adjusted_type, (_type))); \
    } while (0)

DEFINE_CASE(ast_type_compatibility1, "AST Types - function paramter adjusted types")
    struct kefir_ast_type_storage type_storage;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_storage_init(&type_storage, &symbols));

    struct kefir_ast_function_type *function_type = NULL;
    const struct kefir_ast_type *type = kefir_ast_type_function(&kft_mem, &type_storage, kefir_ast_type_void(), "", &function_type);
    ASSERT(type != NULL);
    ASSERT(function_type != NULL);

    struct kefir_ast_function_type *function_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(&kft_mem, &type_storage, kefir_ast_type_void(), "", &function_type2);
    ASSERT(type2 != NULL);
    ASSERT(function_type2 != NULL);

    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_storage, function_type, "param1",
        kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_storage, function_type, "param2",
        kefir_ast_type_pointer(&kft_mem, &type_storage, kefir_ast_type_signed_int()), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_storage, function_type, "param3",
        kefir_ast_type_array(&kft_mem, &type_storage, kefir_ast_type_signed_int(), 10, NULL), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_storage, function_type, "param4",
        kefir_ast_type_array(&kft_mem, &type_storage, kefir_ast_type_signed_int(), 11, NULL), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_storage, function_type, "param5",
        kefir_ast_type_array(&kft_mem, &type_storage, kefir_ast_type_signed_int(), 12, &(const struct kefir_ast_type_qualification){
            .constant = false,
            .restricted = false,
            .volatile_type = false
        }), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_storage, function_type, "param6",
        kefir_ast_type_array(&kft_mem, &type_storage, kefir_ast_type_signed_int(), 13, &(const struct kefir_ast_type_qualification){
            .constant = true,
            .restricted = true,
            .volatile_type = false
        }), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_storage, function_type, "param7", type2, NULL));

    ASSERT_FUNCTION_PARAM_ADJ_TYPE(function_type, 0, kefir_ast_type_signed_int());
    ASSERT_FUNCTION_PARAM_ADJ_TYPE(function_type, 1, kefir_ast_type_pointer(&kft_mem, &type_storage, kefir_ast_type_signed_int()));
    ASSERT_FUNCTION_PARAM_ADJ_TYPE(function_type, 2, kefir_ast_type_pointer(&kft_mem, &type_storage, kefir_ast_type_signed_int()));
    ASSERT_FUNCTION_PARAM_ADJ_TYPE(function_type, 3, kefir_ast_type_pointer(&kft_mem, &type_storage, kefir_ast_type_signed_int()));
    ASSERT_FUNCTION_PARAM_ADJ_TYPE(function_type, 4, kefir_ast_type_pointer(&kft_mem, &type_storage, kefir_ast_type_signed_int()));
    ASSERT_FUNCTION_PARAM_ADJ_TYPE(function_type, 5, kefir_ast_type_qualified(&kft_mem, &type_storage,
        kefir_ast_type_pointer(&kft_mem, &type_storage, kefir_ast_type_signed_int()), (const struct kefir_ast_type_qualification){
            .constant = true,
            .restricted = true,
            .volatile_type = false
        }));
    ASSERT_FUNCTION_PARAM_ADJ_TYPE(function_type, 6, kefir_ast_type_pointer(&kft_mem, &type_storage, type2));

    ASSERT_OK(kefir_ast_type_storage_free(&kft_mem, &type_storage));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_type_basic_compatibility, "AST Types - basic type compatibility")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    const struct kefir_ast_type *BASIC_TYPES[] = {
        kefir_ast_type_void(),
        kefir_ast_type_bool(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),
        kefir_ast_type_unsigned_short(),
        kefir_ast_type_signed_short(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double()
    };
    const kefir_size_t BASIC_TYPES_LENGTH = sizeof(BASIC_TYPES) / sizeof(BASIC_TYPES[0]);
    for (kefir_size_t i = 0; i < BASIC_TYPES_LENGTH; i++) {
        ASSERT(BASIC_TYPES[i] != NULL);
        for (kefir_size_t j = 0; j < BASIC_TYPES_LENGTH; j++) {
            ASSERT(BASIC_TYPES[j] != NULL);
            if (i != j) {
                ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, BASIC_TYPES[i], BASIC_TYPES[j]));
            } else {
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, BASIC_TYPES[i], BASIC_TYPES[j]));
            }
        }
    }
END_CASE

static const struct kefir_ast_type *build_enum_type(struct kefir_mem *mem,
                                                  struct kefir_symbol_table *symbols,
                                                  struct kefir_ast_type_storage *type_storage,
                                                  struct kefir_ast_enum_type **enum1_type) {
    const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(&kft_mem, type_storage, "enum1", enum1_type);
    ASSERT_OK(kefir_ast_enumeration_type_constant(mem, symbols, *enum1_type, "c1",
    kefir_ast_constant_expression_integer(mem, 10)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(mem, symbols, *enum1_type, "c2"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(mem, symbols, *enum1_type, "c3"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(mem, symbols, *enum1_type, "c4",
        kefir_ast_constant_expression_integer(mem, 1)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(mem, symbols, *enum1_type, "c5"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(mem, symbols, *enum1_type, "c10",
        kefir_ast_constant_expression_integer(mem, 0)));
    return type1;
}

DEFINE_CASE(ast_type_enum_compatibility, "AST Types - enumeration compatibility")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_type_storage type_storage;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_storage_init(&type_storage, &symbols));

    struct kefir_ast_enum_type *enum1_type;
    const struct kefir_ast_type *enum1 = build_enum_type(&kft_mem, &symbols, &type_storage, &enum1_type);
    struct kefir_ast_enum_type *enum2_type;
    const struct kefir_ast_type *enum2 = build_enum_type(&kft_mem, &symbols, &type_storage, &enum2_type);
    struct kefir_ast_enum_type *enum3_type;
    const struct kefir_ast_type *enum3 = build_enum_type(&kft_mem, &symbols, &type_storage, &enum3_type);
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, &symbols, enum3_type, "c20",
        kefir_ast_constant_expression_integer(&kft_mem, 1000)));

    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum1, enum2));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum2, enum1));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum1, enum3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum3, enum1));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum2, enum3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum3, enum1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum1, kefir_ast_enumeration_underlying_type(enum1_type)));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, kefir_ast_enumeration_underlying_type(enum1_type), enum1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum2, kefir_ast_enumeration_underlying_type(enum2_type)));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, kefir_ast_enumeration_underlying_type(enum2_type), enum2));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum3, kefir_ast_enumeration_underlying_type(enum3_type)));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, kefir_ast_enumeration_underlying_type(enum3_type), enum3));

    ASSERT_OK(kefir_ast_type_storage_free(&kft_mem, &type_storage));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE