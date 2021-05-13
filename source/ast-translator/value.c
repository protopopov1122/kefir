#include "kefir/ast-translator/value.h"
#include "kefir/ast-translator/util.h"
#include "kefir/ast-translator/type_resolver.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t load_bitfield(struct kefir_mem *mem,
                                  struct kefir_ast_translator_context *context,
                                  struct kefir_irbuilder_block *builder,
                                  const struct kefir_ast_struct_member *node,
                                  struct kefir_ast_type_layout **layout,
                                  const struct kefir_ir_typeentry **typeentry_ptr) {
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
    ASSIGN_PTR(layout, member_layout);

    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(cached_type->object.ir_type, member_layout->value);
    ASSIGN_PTR(typeentry_ptr, typeentry);
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

        case KEFIR_IR_TYPE_BITS: {
            kefir_size_t bits = 0;
            kefir_size_t pad = 0;
            KEFIR_IR_BITS_PARAM_GET(typeentry->param, NULL, &bits, &pad)

            kefir_size_t bitwidth = bits + pad;
            if (bitwidth <= 8) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8U, 0));
            } else if  (bitwidth <= 16) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD16U, 0));
            } else if  (bitwidth <= 24) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD24U, 0));
            } else if  (bitwidth <= 32) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD32U, 0));
            } else if  (bitwidth <= 40) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD40U, 0));
            } else if  (bitwidth <= 48) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD48U, 0));
            } else if  (bitwidth <= 56) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD56U, 0));
            } else if  (bitwidth <= 64) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD64, 0));
            } else {
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Bit-field exceeds storage unit width");
            }
            if (pad > 0) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, pad));
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_IRSHIFT, 0));
            }
        } break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected bit-field storage unit type");
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_bitfield(struct kefir_mem *mem,
                                     struct kefir_ast_translator_context *context,
                                     struct kefir_irbuilder_block *builder,
                                     const struct kefir_ast_struct_member *node) {
    struct kefir_ast_type_layout *member_layout = NULL;
    REQUIRE_OK(load_bitfield(mem, context, builder, node, &member_layout, NULL));

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

static kefir_result_t store_bitfield(struct kefir_mem *mem,
                                   struct kefir_ast_translator_context *context,
                                   struct kefir_irbuilder_block *builder,
                                   const struct kefir_ast_struct_member *node) {
    struct kefir_ast_type_layout *member_layout = NULL;
    const struct kefir_ir_typeentry *typeentry = NULL;

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PICK, 1));
    REQUIRE_OK(load_bitfield(mem, context, builder, node, &member_layout, &typeentry));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_XCHG, 1));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_INSERTBITS,
        member_layout->bitfield_props.offset, member_layout->bitfield_props.width));
    
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_INT8:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE8, 0));
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE16, 0));
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE32, 0));
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_INT64:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE64, 0));
            break;

        case KEFIR_IR_TYPE_BITS: {
            kefir_size_t bits = 0;
            kefir_size_t pad = 0;
            KEFIR_IR_BITS_PARAM_GET(typeentry->param, NULL, &bits, &pad)

            if (pad > 0) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, pad));
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_ILSHIFT, 0));
            }

            kefir_size_t bitwidth = bits + pad;
            if (bitwidth <= 8) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE8, 0));
            } else if  (bitwidth <= 16) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE16, 0));
            } else if  (bitwidth <= 24) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE24, 0));
            } else if  (bitwidth <= 32) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE32, 0));
            } else if  (bitwidth <= 40) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE40, 0));
            } else if  (bitwidth <= 48) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE48, 0));
            } else if  (bitwidth <= 56) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE56, 0));
            } else if  (bitwidth <= 64) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE64, 0));
            } else {
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Bit-field exceeds storage unit width");
            }
        } break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected bit-field storage unit type");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_resolve_node_value(struct kefir_mem *mem,
                                                   struct kefir_ast_translator_context *context,
                                                   struct kefir_irbuilder_block *builder,
                                                   const struct kefir_ast_node_base *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    
    if (node->properties.expression_props.bitfield) {
        REQUIRE(node->klass->type == KEFIR_AST_STRUCTURE_MEMBER ||
            node->klass->type == KEFIR_AST_STRUCTURE_INDIRECT_MEMBER,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected bit-field node to be a direct/indirect structure member"));
        REQUIRE_OK(resolve_bitfield(mem, context, builder, (const struct kefir_ast_struct_member *) node->self));
    } else {
        REQUIRE_OK(kefir_ast_translator_load_value(node->properties.type, context->ast_context->type_traits, builder));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_store_node_value(struct kefir_mem *mem,
                                                 struct kefir_ast_translator_context *context,
                                                 struct kefir_irbuilder_block *builder,
                                                 const struct kefir_ast_node_base *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));

    if (node->properties.expression_props.bitfield) {
        REQUIRE(node->klass->type == KEFIR_AST_STRUCTURE_MEMBER ||
            node->klass->type == KEFIR_AST_STRUCTURE_INDIRECT_MEMBER,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected bit-field node to be a direct/indirect structure member"));
        REQUIRE_OK(store_bitfield(mem, context, builder, (const struct kefir_ast_struct_member *) node->self));
    } else {
        REQUIRE_OK(kefir_ast_translator_store_value(mem, node->properties.type, context, builder));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_load_value(const struct kefir_ast_type *type,
                                           const struct kefir_ast_type_traits *type_traits,
                                           struct kefir_irbuilder_block *builder) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));

    const struct kefir_ast_type *normalizer = kefir_ast_translator_normalize_type(type);

    switch (normalizer->tag) {
        case KEFIR_AST_TYPE_VOID:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot load variable with void type");
            
        case KEFIR_AST_TYPE_SCALAR_BOOL:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8U, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_CHAR:
            if (type_traits->character_type_signedness) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8I, 0));
            } else {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8U, 0));
            }
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8U, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8I, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD16U, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD16I, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD32U, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD32I, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
        case KEFIR_AST_TYPE_SCALAR_POINTER:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD64, 0));
            break;
            
        case KEFIR_AST_TYPE_ENUMERATION:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected enumeration type");
            
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
        case KEFIR_AST_TYPE_ARRAY:
        case KEFIR_AST_TYPE_FUNCTION:
            // Intentionally left blank
            break;
            
        case KEFIR_AST_TYPE_QUALIFIED:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected qualified type");
            
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_store_value(struct kefir_mem *mem,
                                            const struct kefir_ast_type *type,
                                            struct kefir_ast_translator_context *context,
                                            struct kefir_irbuilder_block *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));

    const struct kefir_ast_type *normalizer = kefir_ast_translator_normalize_type(type);

    switch (normalizer->tag) {
        case KEFIR_AST_TYPE_VOID:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot store value with void type");
            
        case KEFIR_AST_TYPE_SCALAR_BOOL:
        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE8, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE16, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE32, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
        case KEFIR_AST_TYPE_SCALAR_POINTER:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE64, 0));
            break;
            
        case KEFIR_AST_TYPE_ENUMERATION:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected enumeration type");
            
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
        case KEFIR_AST_TYPE_ARRAY: {
            const struct kefir_ast_translator_resolved_type *cached_type = NULL;
            REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(mem, &context->type_cache.resolver, context->environment, context->module,
                type, 0, &cached_type));
            REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected cached type to be an object"));
            
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_BCOPY,
                cached_type->object.ir_type_id, cached_type->object.layout->value));
        } break;

        case KEFIR_AST_TYPE_FUNCTION:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot store value with function type");
            
        case KEFIR_AST_TYPE_QUALIFIED:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected qualified type");
            
    }
    return KEFIR_OK;
}