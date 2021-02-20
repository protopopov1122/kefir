#include "kefir/ast/translator/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_stored_object_type(const struct kefir_ast_type *type,
                                    struct kefir_irbuilder_type *builder,
                                    const kefir_id_t *type_identifier) {
    UNUSED(type_identifier);
    switch (type->tag) {
        case KEFIR_AST_TYPE_VOID:
            return KEFIR_OK;

        case KEFIR_AST_TYPE_SCALAR_BOOL:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_BOOL, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_CHAR, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_CHAR, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_SHORT, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_SHORT, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_INT, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_INT, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_LONG, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_LONG, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_LONG, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_LONG, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_FLOAT32, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_FLOAT64, 0, 0);

        case KEFIR_AST_TYPE_SCALAR_POINTER:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_WORD, 0, 0);

        case KEFIR_AST_TYPE_ENUMERATION:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_INT, 0, 0);

        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION: {
            REQUIRE(type->structure_type.complete,
                KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-complete structure/union definitions are not supported yet"));
            KEFIR_IRBUILDER_TYPE_APPEND_V(builder,
                type->tag == KEFIR_AST_TYPE_STRUCTURE
                    ? KEFIR_IR_TYPE_STRUCT
                    : KEFIR_IR_TYPE_UNION,
                0,
                kefir_list_length(&type->structure_type.fields));
            for (const struct kefir_list_entry *iter = kefir_list_head(&type->structure_type.fields);
                iter != NULL;
                kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_struct_field *, field,
                    iter->value);
                REQUIRE_OK(kefir_ast_translate_stored_object_type(field->type, builder, type_identifier));
            }
        } break;

        case KEFIR_AST_TYPE_ARRAY: {
            switch (type->array_type.boundary) {
                case KEFIR_AST_ARRAY_UNBOUNDED:
                    return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_WORD, 0, 0);

                case KEFIR_AST_ARRAY_BOUNDED:
                    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_ARRAY, 0, type->array_type.length));
                    return kefir_ast_translate_stored_object_type(type->array_type.element_type, builder, type_identifier);

                case KEFIR_AST_ARRAY_BOUNDED_STATIC:
                case KEFIR_AST_ARRAY_VLA_STATIC:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Static array type is not supported in that context");

                case KEFIR_AST_ARRAY_VLA:
                    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Variable-length arrays are not supported yet");

            } 
        } break;

        case KEFIR_AST_TYPE_QUALIFIED:
            return kefir_ast_translate_stored_object_type(type->qualified_type.type, builder, type_identifier);

        case KEFIR_AST_TYPE_FUNCTION:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot translate AST function type into IR type");
    }
    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST type tag");
}