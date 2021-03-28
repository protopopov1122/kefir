#include "kefir/ast/translator/translator.h"
#include "kefir/ast/alignment.h"
#include "kefir/ast/translator/alignment.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_stored_object_type(struct kefir_mem *mem,
                                                  const struct kefir_ast_type *type,
                                                  kefir_size_t alignment,
                                                  const struct kefir_ast_translator_environment *env,
                                                  struct kefir_irbuilder_type *builder) {
    switch (type->tag) {
        case KEFIR_AST_TYPE_VOID:
            return KEFIR_OK;

        case KEFIR_AST_TYPE_SCALAR_BOOL:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_BOOL, alignment, 0);

        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_CHAR, alignment, 0);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_SHORT, alignment, 0);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
        case KEFIR_AST_TYPE_ENUMERATION:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_INT, alignment, 0);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_LONG, alignment, 0);

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_FLOAT32, alignment, 0);

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_FLOAT64, alignment, 0);

        case KEFIR_AST_TYPE_SCALAR_POINTER:
            return KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_WORD, alignment, 0);

        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION: {
            REQUIRE(type->structure_type.complete,
                KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-complete structure/union definitions are not supported yet"));
            KEFIR_IRBUILDER_TYPE_APPEND_V(builder,
                type->tag == KEFIR_AST_TYPE_STRUCTURE
                    ? KEFIR_IR_TYPE_STRUCT
                    : KEFIR_IR_TYPE_UNION,
                alignment,
                kefir_list_length(&type->structure_type.fields));
            for (const struct kefir_list_entry *iter = kefir_list_head(&type->structure_type.fields);
                iter != NULL;
                kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_struct_field *, field,
                    iter->value);
                REQUIRE_OK(kefir_ast_translate_stored_object_type(mem, field->type, field->alignment->value, env, builder));
            }
            return KEFIR_OK;
        }

        case KEFIR_AST_TYPE_ARRAY: {
            switch (type->array_type.boundary) {
                case KEFIR_AST_ARRAY_UNBOUNDED:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot translate unbounded array type to IR type");

                case KEFIR_AST_ARRAY_BOUNDED:
                    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_ARRAY, alignment,
                        kefir_ast_type_array_const_length(&type->array_type)));
                    return kefir_ast_translate_stored_object_type(mem, type->array_type.element_type, KEFIR_AST_DEFAULT_ALIGNMENT, env, builder);

                case KEFIR_AST_ARRAY_BOUNDED_STATIC:
                case KEFIR_AST_ARRAY_VLA_STATIC:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Static array type is not supported in that context");

                case KEFIR_AST_ARRAY_VLA:
                    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Variable-length arrays are not supported yet");

            } 
        } break;

        case KEFIR_AST_TYPE_QUALIFIED:
            return kefir_ast_translate_stored_object_type(mem, type->qualified_type.type, alignment, env, builder);

        case KEFIR_AST_TYPE_FUNCTION:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot translate AST function type into IR type");
    }
    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST type tag");
}