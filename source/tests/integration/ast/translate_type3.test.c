#include <stdio.h>
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/layout.h"
#include "kefir/test/util.h"
#include "kefir/util/json.h"

static kefir_result_t dump_type_layout(struct kefir_json_output *json,
                                     const struct kefir_ast_type_layout *layout) {
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "index"));
    REQUIRE_OK(kefir_json_output_uinteger(json, layout->value));
    REQUIRE_OK(kefir_json_output_object_key(json, "size"));
    REQUIRE_OK(kefir_json_output_uinteger(json, layout->properties.size));
    REQUIRE_OK(kefir_json_output_object_key(json, "alignment"));
    REQUIRE_OK(kefir_json_output_uinteger(json, layout->properties.alignment));
    REQUIRE_OK(kefir_json_output_object_key(json, "relative_offset"));
    REQUIRE_OK(kefir_json_output_uinteger(json, layout->properties.relative_offset));
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    switch (layout->type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION: {
            REQUIRE_OK(kefir_json_output_string(json, layout->type->tag == KEFIR_AST_TYPE_STRUCTURE
                ? "struct"
                : "union"));
            REQUIRE_OK(kefir_json_output_object_key(json, "fields"));
            REQUIRE_OK(kefir_json_output_array_begin(json));

            for (const struct kefir_list_entry *iter = kefir_list_head(&layout->structure_layout.member_list);
                iter != NULL;
                kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(const struct kefir_ast_type_layout_structure_member *, member,
                    iter->value);
                REQUIRE_OK(kefir_json_output_object_begin(json));
                REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
                if (member->identifier != NULL) {
                    REQUIRE_OK(kefir_json_output_string(json, member->identifier));
                } else {
                    REQUIRE_OK(kefir_json_output_null(json));
                }
                REQUIRE_OK(kefir_json_output_object_key(json, "layout"));
                REQUIRE_OK(dump_type_layout(json, member->layout));
                REQUIRE_OK(kefir_json_output_object_end(json));
            }
            REQUIRE_OK(kefir_json_output_array_end(json));
        } break;

        case KEFIR_AST_TYPE_ARRAY:
            REQUIRE_OK(kefir_json_output_string(json, "array"));
            REQUIRE_OK(kefir_json_output_object_key(json, "element"));
            REQUIRE_OK(dump_type_layout(json, layout->array_layout.element_type));
            break;

        default:
            REQUIRE_OK(kefir_json_output_string(json, "scalar"));
            break;
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t dump_type(struct kefir_mem *mem,
                       struct kefir_json_output *json,
                       const struct kefir_ast_type *type) {
    struct kefir_ir_type ir_type;
    struct kefir_irbuilder_type builder;
    struct kefir_ast_translator_environment env;

    REQUIRE_OK(kefir_ir_type_alloc(mem, 0, &ir_type));
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, &ir_type));
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    
    struct kefir_ast_type_layout *layout1 = NULL;
    REQUIRE_OK(kefir_ast_translate_object_type(mem, type, 0, &env, &builder, &layout1));
    REQUIRE_OK(kefir_ast_translator_evaluate_type_layout(mem, &env, layout1, &ir_type));
    REQUIRE(layout1 != NULL, KEFIR_INTERNAL_ERROR);

    REQUIRE_OK(dump_type_layout(json, layout1));
    REQUIRE_OK(kefir_ast_type_layout_free(mem, layout1));

    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    REQUIRE_OK(kefir_ir_type_free(mem, &ir_type));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, stdout, 4));
    REQUIRE_OK(kefir_json_output_array_begin(&json));

    const struct kefir_ast_type *type1 = kefir_ast_type_signed_long();
    REQUIRE_OK(dump_type(mem, &json, type1));

    const struct kefir_ast_type *type2 = kefir_ast_type_array(mem, &type_bundle,
        kefir_ast_type_char(), kefir_ast_constant_expression_integer(mem, 10), NULL);
    REQUIRE_OK(dump_type(mem, &json, type2));

    const struct kefir_ast_type *type3 = kefir_ast_type_array(mem, &type_bundle,
        kefir_ast_type_array(mem, &type_bundle, kefir_ast_type_char(),
            kefir_ast_constant_expression_integer(mem, 5), NULL),
        kefir_ast_constant_expression_integer(mem, 10), NULL);
    REQUIRE_OK(dump_type(mem, &json, type3));

    struct kefir_ast_enum_type *enum_type_4_0 = NULL;
    const struct kefir_ast_type *type4_0 = kefir_ast_type_enumeration(mem, &type_bundle,
        "", kefir_ast_type_signed_int(), &enum_type_4_0);

    struct kefir_ast_struct_type *struct_type4_1 = NULL;
    const struct kefir_ast_type *type4_1 = kefir_ast_type_structure(mem, &type_bundle,
        "", &struct_type4_1);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4_1,
        "field1", kefir_ast_type_char(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4_1,
        "field2", kefir_ast_type_qualified(mem, &type_bundle,
            kefir_ast_type_unsigned_long(), (struct kefir_ast_type_qualification){
                .constant = true
            }), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4_1,
        "field3", kefir_ast_type_array(mem, &type_bundle, type4_0,
            kefir_ast_constant_expression_integer(mem, 15), NULL), NULL));

    struct kefir_ast_struct_type *struct_type4 = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_structure(mem, &type_bundle,
        "", &struct_type4);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "x", kefir_ast_type_float(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "y",
        kefir_ast_type_pointer(mem, &type_bundle, kefir_ast_type_double()), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "z",
        kefir_ast_type_signed_short(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "a",
        kefir_ast_type_array(mem, &type_bundle,
            kefir_ast_type_signed_int(), kefir_ast_constant_expression_integer(mem, 1), NULL), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "b",
        type4_1, NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, NULL,
        type4_1, NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "c",
        kefir_ast_type_array(mem, &type_bundle,
            type4_1, kefir_ast_constant_expression_integer(mem, 10), NULL), NULL));
    REQUIRE_OK(dump_type(mem, &json, type4));

    REQUIRE_OK(kefir_json_output_array_end(&json));
    REQUIRE_OK(kefir_json_output_finalize(&json));

    REQUIRE_OK(kefir_ast_type_bundle_free(mem, &type_bundle));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
