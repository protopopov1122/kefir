#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/value.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t resolve_bitfield(struct kefir_mem *mem,
                                     struct kefir_ast_translator_context *context,
                                     struct kefir_irbuilder_block *builder,
                                     const struct kefir_ast_struct_member *node) {
    const struct kefir_ast_type *structure_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle,
        node->structure->properties.type);
    if (structure_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        structure_type = kefir_ast_unqualified_type(structure_type->referenced_type);
    }

    const struct kefir_ast_translator_resolved_type *cached_type = NULL;
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(mem, &context->type_cache.resolver, context->environment, context->module,
        structure_type, 0, &cached_type));
    REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected cached type to be an object"));

    struct kefir_ast_type_layout *member_layout = NULL;
    struct kefir_ast_designator designator = {
        .type = KEFIR_AST_DESIGNATOR_MEMBER,
        .member = node->member,
        .next = NULL
    };
    REQUIRE_OK(kefir_ast_type_layout_resolve(cached_type->object.layout, &designator, &member_layout, NULL, NULL));

    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(cached_type->object.ir_type, member_layout->value);
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_INT8:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8U, 0));
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD16U, 0));
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD32U, 0));
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_INT64:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD64, 0));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected bit-field storage unit type");
    }

    if (KEFIR_AST_TYPE_IS_SIGNED_INTEGER(member_layout->type)) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_EXTSBITS,
            member_layout->bitfield_props.offset, member_layout->bitfield_props.width));
    } else if (KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(member_layout->type)) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_EXTUBITS,
            member_layout->bitfield_props.offset, member_layout->bitfield_props.width));
    } else {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected bit-field type");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_struct_member_node(struct kefir_mem *mem,
                                                  struct kefir_ast_translator_context *context,
                                                  struct kefir_irbuilder_block *builder,
                                                  const struct kefir_ast_struct_member *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST struct member node"));
    
    REQUIRE_OK(kefir_ast_translate_struct_member_lvalue(mem, context, builder, node));
    
    if (node->base.properties.expression_props.bitfield) {
        REQUIRE_OK(resolve_bitfield(mem, context, builder, node));
    } else {
        REQUIRE_OK(kefir_ast_translator_load_value(node->base.properties.type, context->ast_context->type_traits, builder));
    }
    return KEFIR_OK;
}