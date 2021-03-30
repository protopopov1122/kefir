#include <stdio.h>
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast/translator/translator.h"
#include "kefir/test/util.h"

static kefir_result_t dump_type_layout(FILE *out, kefir_size_t indent, const char *prefix, const struct kefir_ast_type_layout *layout) {
    for (kefir_size_t i = 0; i < indent; i++) {
        fprintf(out, "\t");
    }
    if (prefix != NULL) {
        fprintf(out, "%s: ", prefix);
    }
    switch (layout->type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION: {
            fprintf(out, "%s("KEFIR_SIZE_FMT")\n",
                layout->type->tag == KEFIR_AST_TYPE_STRUCTURE
                    ? "STRUCT"
                    : "UNION",
                layout->value);
            struct kefir_hashtree_node_iterator iter;
            for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&layout->structure_layout.members, &iter);
                node != NULL;
                node = kefir_hashtree_next(&iter)) {
                ASSIGN_DECL_CAST(const struct kefir_ast_type_layout *, member,
                    node->value);
                REQUIRE_OK(dump_type_layout(out, indent + 1, (const char *) node->key, member));
            }
        } break;

        case KEFIR_AST_TYPE_ARRAY:
            fprintf(out, "ARRAY("KEFIR_SIZE_FMT")\n", layout->value);
            REQUIRE_OK(dump_type_layout(out, indent + 1, NULL, layout->array_layout.element_type));
            break;

        default:
            fprintf(out, "SCALAR("KEFIR_SIZE_FMT")\n", layout->value);
            break;
    }
    return KEFIR_OK;
}

kefir_result_t dump_type(struct kefir_mem *mem,
                       const struct kefir_ast_type *type) {
    struct kefir_ir_type ir_type;
    struct kefir_irbuilder_type builder;
    struct kefir_ast_translator_environment env;

    REQUIRE_OK(kefir_ir_type_alloc(mem, 0, &ir_type));
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, &ir_type));
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    
    struct kefir_ast_type_layout *layout1 = NULL;
    REQUIRE_OK(kefir_ast_translate_object_type(mem, type, 0, &env, &builder, &layout1));
    REQUIRE(layout1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(dump_type_layout(stdout, 0, NULL, layout1));
    REQUIRE_OK(kefir_ast_type_layout_free(mem, layout1));

    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    REQUIRE_OK(kefir_ir_type_free(mem, &ir_type));
    printf("\n");
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *type1 = kefir_ast_type_signed_long();
    REQUIRE_OK(dump_type(mem, type1));

    const struct kefir_ast_type *type2 = kefir_ast_type_array(mem, &type_bundle,
        kefir_ast_type_char(), kefir_ast_constant_expression_integer(mem, 10), NULL);
    REQUIRE_OK(dump_type(mem, type2));

    const struct kefir_ast_type *type3 = kefir_ast_type_array(mem, &type_bundle,
        kefir_ast_type_array(mem, &type_bundle, kefir_ast_type_char(),
            kefir_ast_constant_expression_integer(mem, 5), NULL),
        kefir_ast_constant_expression_integer(mem, 10), NULL);
    REQUIRE_OK(dump_type(mem, type3));

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
    REQUIRE_OK(dump_type(mem, type4));

    REQUIRE_OK(kefir_ast_type_bundle_free(mem, &type_bundle));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}