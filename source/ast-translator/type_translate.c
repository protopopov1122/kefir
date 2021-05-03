#include "kefir/ast-translator/translator.h"
#include "kefir/ast/alignment.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t translate_scalar_type(struct kefir_mem *mem,
                                          const struct kefir_ast_type *type,
                                          kefir_size_t alignment,
                                          struct kefir_irbuilder_type *builder,
                                          struct kefir_ast_type_layout **layout_ptr) {
    kefir_size_t type_index = kefir_ir_type_total_length(builder->type);
    switch (type->tag) {
        case KEFIR_AST_TYPE_VOID:
            break;

        case KEFIR_AST_TYPE_SCALAR_BOOL:
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_BOOL, alignment, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_CHAR, alignment, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_SHORT, alignment, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
        case KEFIR_AST_TYPE_ENUMERATION:
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_INT, alignment, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_LONG, alignment, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_FLOAT32, alignment, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_FLOAT64, alignment, 0));
            break;

        case KEFIR_AST_TYPE_SCALAR_POINTER:
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_WORD, alignment, 0));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Not a scalar type");
    }

    if (layout_ptr != NULL) {
        *layout_ptr = kefir_ast_new_type_layout(mem, type, alignment, type_index);
        REQUIRE(*layout_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST type layout"));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_array_type(struct kefir_mem *mem,
                                          const struct kefir_ast_type *type,
                                          kefir_size_t alignment,
                                          const struct kefir_ast_translator_environment *env,
                                          struct kefir_irbuilder_type *builder,
                                          struct kefir_ast_type_layout **layout_ptr) {
    kefir_size_t type_index = kefir_ir_type_total_length(builder->type);
    struct kefir_ast_type_layout *element_layout = NULL;

    switch (type->array_type.boundary) {
        case KEFIR_AST_ARRAY_UNBOUNDED: {
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_ARRAY, alignment,
                0));
            REQUIRE_OK(kefir_ast_translate_object_type(mem,
                type->array_type.element_type, KEFIR_AST_DEFAULT_ALIGNMENT, env, builder,
                layout_ptr != NULL ? &element_layout : NULL));
            if (layout_ptr != NULL) {
                *layout_ptr = kefir_ast_new_type_layout(mem, type, alignment, type_index);
                REQUIRE(*layout_ptr != NULL,
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST type layout"));
                (*layout_ptr)->array_layout.element_type = element_layout;
            }
        } break;

        case KEFIR_AST_ARRAY_BOUNDED:
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_ARRAY, alignment,
                kefir_ast_type_array_const_length(&type->array_type)));
            REQUIRE_OK(kefir_ast_translate_object_type(mem,
                type->array_type.element_type, KEFIR_AST_DEFAULT_ALIGNMENT, env, builder,
                layout_ptr != NULL ? &element_layout : NULL));
            if (layout_ptr != NULL) {
                *layout_ptr = kefir_ast_new_type_layout(mem, type, alignment, type_index);
                REQUIRE(*layout_ptr != NULL,
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST type layout"));
                (*layout_ptr)->array_layout.element_type = element_layout;
            }
            break;

        case KEFIR_AST_ARRAY_BOUNDED_STATIC:
        case KEFIR_AST_ARRAY_VLA_STATIC:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Static array type is not supported in that context");

        case KEFIR_AST_ARRAY_VLA:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Variable-length arrays are not supported yet");
    }
    return KEFIR_OK;
}

static kefir_result_t translate_struct_type(struct kefir_mem *mem,
                                          const struct kefir_ast_type *type,
                                          kefir_size_t alignment,
                                          const struct kefir_ast_translator_environment *env,
                                          struct kefir_irbuilder_type *builder,
                                          struct kefir_ast_type_layout **layout_ptr) {
    REQUIRE(type->structure_type.complete,
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-complete structure/union definitions are not supported yet"));

    kefir_size_t type_index = kefir_ir_type_total_length(builder->type);
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder,
        type->tag == KEFIR_AST_TYPE_STRUCTURE
            ? KEFIR_IR_TYPE_STRUCT
            : KEFIR_IR_TYPE_UNION,
        alignment,
        kefir_list_length(&type->structure_type.fields)));

    kefir_bool_t allocated = false;
    struct kefir_ast_type_layout *layout = NULL;
    if (layout_ptr != NULL) {
        layout = kefir_ast_new_type_layout(mem, type, alignment, type_index);
        REQUIRE(layout != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST type layout"));
        allocated = true;
        *layout_ptr = layout;
    }
    
    kefir_result_t res = KEFIR_OK;
    for (const struct kefir_list_entry *iter = kefir_list_head(&type->structure_type.fields);
        iter != NULL && res == KEFIR_OK;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_struct_field *, field,
            iter->value);

        struct kefir_ast_type_layout *element_layout = NULL;        
        res = kefir_ast_translate_object_type(mem, field->type,
            field->alignment->value, env, builder,
            layout != NULL ? &element_layout : NULL);
        if (res == KEFIR_OK && element_layout != NULL) {
            layout->bitfield = false; // TODO Implement bit-field allocation
            layout->bitfield_props.offset = 0;
            layout->bitfield_props.width = 0;

            if (field->identifier == NULL || strlen(field->identifier) == 0) {
                res = kefir_ast_type_layout_add_structure_anonymous_member(mem, layout, element_layout);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    kefir_ast_type_layout_free(mem, element_layout);
                });
            } else {
                res = kefir_ast_type_layout_insert_structure_member(mem, layout, field->identifier, element_layout);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    kefir_ast_type_layout_free(mem, element_layout);
                });
            }
        }
    }
    
    if (res != KEFIR_OK && allocated) {
        kefir_ast_type_layout_free(mem, layout);
        *layout_ptr = NULL;
    }
    return res;
}

kefir_result_t kefir_ast_translate_object_type(struct kefir_mem *mem,
                                           const struct kefir_ast_type *type,
                                           kefir_size_t alignment,
                                           const struct kefir_ast_translator_environment *env,
                                           struct kefir_irbuilder_type *builder,
                                           struct kefir_ast_type_layout **layout_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type builder"));

    switch (type->tag) {
        case KEFIR_AST_TYPE_VOID:
        case KEFIR_AST_TYPE_SCALAR_BOOL:
        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_FLOAT:
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
        case KEFIR_AST_TYPE_SCALAR_POINTER:
            REQUIRE_OK(translate_scalar_type(mem, type, alignment, builder, layout_ptr));
            break;

        case KEFIR_AST_TYPE_ENUMERATION:
            REQUIRE_OK(kefir_ast_translate_object_type(mem, type->enumeration_type.underlying_type,
                alignment, env, builder, layout_ptr));
            break;

        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
            REQUIRE_OK(translate_struct_type(mem, type, alignment, env, builder, layout_ptr));
            break;

        case KEFIR_AST_TYPE_ARRAY:
            REQUIRE_OK(translate_array_type(mem, type, alignment, env, builder, layout_ptr));
            break;

        case KEFIR_AST_TYPE_FUNCTION:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot translate AST function type into IR type");

        case KEFIR_AST_TYPE_QUALIFIED:
            REQUIRE_OK(kefir_ast_translate_object_type(mem, type->qualified_type.type,
                alignment, env, builder, layout_ptr));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST type tag");
    }

    return KEFIR_OK;
}