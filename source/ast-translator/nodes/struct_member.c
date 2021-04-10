#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

// TODO Bitfield support

static kefir_result_t translate_struct_member(struct kefir_mem *mem,
                                            struct kefir_ast_translator_context *context,
                                            struct kefir_irbuilder_block *builder,
                                            const struct kefir_ast_struct_member *node,
                                            kefir_id_t type_id,
                                            struct kefir_ast_type_layout *type_layout) {
    struct kefir_ast_type_layout *member_layout = NULL;
    struct kefir_ast_designator designator = {
        .type = KEFIR_AST_DESIGNATOR_MEMBER,
        .member = node->member,
        .next = NULL
    };
    REQUIRE_OK(kefir_ast_type_layout_resolve(type_layout, &designator, &member_layout, NULL, NULL));

    REQUIRE_OK(kefir_ast_translate_expression(mem, node->structure, builder, context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR, type_id, member_layout->value));
    REQUIRE_OK(kefir_ast_translator_resolve_value(node->base.properties.type, builder));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_struct_member_node(struct kefir_mem *mem,
                                                  struct kefir_ast_translator_context *context,
                                                  struct kefir_irbuilder_block *builder,
                                                  const struct kefir_ast_struct_member *node) {
    UNUSED(mem);
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST struct member node"));

    const struct kefir_ast_type *structure_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle,
        node->structure->properties.type);
    if (structure_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        structure_type = kefir_ast_unqualified_type(structure_type->referenced_type);
    }

    const struct kefir_ast_translator_cached_type *cached_type = NULL;
    REQUIRE_OK(kefir_ast_translator_type_cache_generate_owned(mem, structure_type, 0,
        &context->type_cache, context->environment, context->module, &cached_type));

    REQUIRE_OK(translate_struct_member(mem, context, builder, node, cached_type->ir_type_id, cached_type->type_layout));
    return KEFIR_OK;
}