#include "kefir/ast-translator/translator.h"
#include "kefir/ast/alignment.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t scalar_typeentry(const struct kefir_ast_type *type,
                                     kefir_size_t alignment,
                                     struct kefir_ir_typeentry *typeentry) {
    typeentry->alignment = alignment;
    typeentry->param = 0;
    switch (type->tag) {
        case KEFIR_AST_TYPE_VOID:
            break;

        case KEFIR_AST_TYPE_SCALAR_BOOL:
            typeentry->typecode = KEFIR_IR_TYPE_BOOL;
            break;

        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            typeentry->typecode = KEFIR_IR_TYPE_CHAR;
            break;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            typeentry->typecode = KEFIR_IR_TYPE_SHORT;
            break;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
        case KEFIR_AST_TYPE_ENUMERATION:
            typeentry->typecode = KEFIR_IR_TYPE_INT;
            break;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
            typeentry->typecode = KEFIR_IR_TYPE_LONG;
            break;

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            typeentry->typecode = KEFIR_IR_TYPE_FLOAT32;
            break;

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            typeentry->typecode = KEFIR_IR_TYPE_FLOAT64;
            break;

        case KEFIR_AST_TYPE_SCALAR_POINTER:
            typeentry->typecode = KEFIR_IR_TYPE_WORD;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Not a scalar type");
    }
    return KEFIR_OK;
}

static kefir_result_t translate_scalar_type(struct kefir_mem *mem,
                                          const struct kefir_ast_type *type,
                                          kefir_size_t alignment,
                                          struct kefir_irbuilder_type *builder,
                                          struct kefir_ast_type_layout **layout_ptr) {
    kefir_size_t type_index = kefir_ir_type_total_length(builder->type);

    if (type->tag != KEFIR_AST_TYPE_VOID) {
        struct kefir_ir_typeentry typeentry = {0};
        REQUIRE_OK(scalar_typeentry(type, alignment, &typeentry));
        REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND(builder, &typeentry));
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

static kefir_result_t insert_struct_field(struct kefir_mem *mem,
                                        struct kefir_ast_struct_field *field,
                                        struct kefir_ast_type_layout *layout,
                                        struct kefir_ast_type_layout *element_layout) {

    if (field->identifier == NULL || strlen(field->identifier) == 0) {
        REQUIRE_OK(kefir_ast_type_layout_add_structure_anonymous_member(mem, layout, element_layout));
    } else {
        REQUIRE_OK(kefir_ast_type_layout_insert_structure_member(mem, layout, field->identifier, element_layout));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_normal_struct_field(struct kefir_mem *mem,
                                                  struct kefir_ast_struct_field *field,
                                                  const struct kefir_ast_translator_environment *env,
                                                  struct kefir_ast_type_layout *layout,
                                                  kefir_size_t type_index,
                                                  struct kefir_irbuilder_type *builder) {
    struct kefir_ast_type_layout *element_layout = NULL;        
    REQUIRE_OK(kefir_ast_translate_object_type(mem, field->type,
        field->alignment->value, env, builder,
        layout != NULL ? &element_layout : NULL));
    if (element_layout != NULL) {
        element_layout->bitfield = field->bitfield;
        element_layout->bitfield_props.offset = 0;
        if (field->bitfield) {
            element_layout->bitfield_props.width = field->bitwidth->value.integer;
        } else {
            element_layout->bitfield_props.width = 0;
        }

        kefir_result_t res = insert_struct_field(mem, field, layout, element_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, element_layout);
            return res;
        });
    }

    kefir_ir_type_at(builder->type, type_index)->param++;
    return KEFIR_OK;
}

struct bitfield_manager {
    struct kefir_ir_bitfield_allocator allocator;
    struct kefir_ast_type_layout *last_bitfield_layout;
    kefir_size_t last_bitfield_storage;
};


static kefir_result_t translate_bitfield(struct kefir_mem *mem,
                                       struct kefir_ast_struct_field *field,
                                       const struct kefir_ast_translator_environment *env,
                                       struct kefir_ast_type_layout *layout,
                                       struct kefir_irbuilder_type *builder,
                                       kefir_size_t type_index,
                                       struct bitfield_manager *bitfield_mgr) {
    if (field->bitwidth->value.integer == 0) {
        REQUIRE_OK(KEFIR_IR_BITFIELD_ALLOCATOR_RESET(&bitfield_mgr->allocator));
        bitfield_mgr->last_bitfield_storage = 0;
        bitfield_mgr->last_bitfield_layout = NULL;
        return KEFIR_OK;
    }

    const struct kefir_ir_bitfield *last_bitfield = NULL;
    REQUIRE_OK(KEFIR_IR_BITFIELD_ALLOCATOR_GET_LAST_BITFIELD(&bitfield_mgr->allocator, &last_bitfield));

    kefir_bool_t allocated = false;
    struct kefir_ir_bitfield ir_bitfield;
    struct kefir_ast_type_layout *element_layout = NULL;        
    if (last_bitfield != NULL) {
        struct kefir_ir_typeentry colocated_typeentry = {0};
        REQUIRE_OK(scalar_typeentry(field->type, field->alignment->value, &colocated_typeentry));
        kefir_result_t res = KEFIR_IR_BITFIELD_ALLOCATOR_NEXT_COLOCATED(mem, &bitfield_mgr->allocator,
            colocated_typeentry.typecode, field->bitwidth->value.integer, &ir_bitfield);
        if (res != KEFIR_OUT_OF_SPACE) {
            REQUIRE_OK(res);
            if (layout != NULL) {
                element_layout = kefir_ast_new_type_layout(mem, field->type, field->alignment->value,
                    bitfield_mgr->last_bitfield_storage);
                REQUIRE(element_layout != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST type layout"));
            }
            allocated = true;
        }
    }
    
    if (!allocated) {
        bitfield_mgr->last_bitfield_storage = kefir_ir_type_total_length(builder->type);
        REQUIRE_OK(kefir_ast_translate_object_type(mem, field->type,
            field->alignment->value, env, builder,
            layout != NULL ? &element_layout : NULL));
        bitfield_mgr->last_bitfield_layout = element_layout;
        REQUIRE_OK(KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(mem, &bitfield_mgr->allocator, type_index,
            bitfield_mgr->last_bitfield_storage, field->bitwidth->value.integer, &ir_bitfield));
        kefir_ir_type_at(builder->type, type_index)->param++;
    }

    if (element_layout != NULL) {
        element_layout->bitfield = true;
        element_layout->bitfield_props.width = ir_bitfield.width;
        element_layout->bitfield_props.offset = ir_bitfield.offset;

        kefir_result_t res = insert_struct_field(mem, field, layout, element_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, element_layout);
            return res;
        });
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
        alignment, 0));

    kefir_bool_t allocated = false;
    struct kefir_ast_type_layout *layout = NULL;
    if (layout_ptr != NULL) {
        layout = kefir_ast_new_type_layout(mem, type, alignment, type_index);
        REQUIRE(layout != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST type layout"));
        allocated = true;
        *layout_ptr = layout;
    }
    
    struct bitfield_manager bitfield_mgr;
    REQUIRE_OK(KEFIR_IR_TARGET_PLATFORM_BITFIELD_ALLOCATOR(mem, env->target_platform, builder->type, &bitfield_mgr.allocator));
    bitfield_mgr.last_bitfield_layout = NULL;
    bitfield_mgr.last_bitfield_storage = 0;

    kefir_result_t res = KEFIR_OK;
    for (const struct kefir_list_entry *iter = kefir_list_head(&type->structure_type.fields);
        iter != NULL && res == KEFIR_OK;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_struct_field *, field,
            iter->value);

        if (field->bitfield && type->tag == KEFIR_AST_TYPE_STRUCTURE) {
            res = translate_bitfield(mem, field, env, layout, builder, type_index, &bitfield_mgr);
        } else {
            REQUIRE_OK(KEFIR_IR_BITFIELD_ALLOCATOR_RESET(&bitfield_mgr.allocator));
            bitfield_mgr.last_bitfield_layout = NULL;
            bitfield_mgr.last_bitfield_storage = 0;
            res = translate_normal_struct_field(mem, field, env, layout, type_index, builder);
        }
    }
    
    REQUIRE_ELSE(res == KEFIR_OK, {
        if (allocated) {
            kefir_ast_type_layout_free(mem, layout);
            *layout_ptr = NULL;
        }
        KEFIR_IR_BITFIELD_ALLOCATOR_FREE(mem, &bitfield_mgr.allocator);
        return res;
    });

    REQUIRE_OK(KEFIR_IR_BITFIELD_ALLOCATOR_FREE(mem, &bitfield_mgr.allocator));
    return KEFIR_OK;
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