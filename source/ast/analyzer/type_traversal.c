#include "kefir/ast/analyzer/type_traversal.h"
#include "kefir/core/linked_stack.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t remove_layer(struct kefir_mem *mem,
                                 struct kefir_list *list,
                                 struct kefir_list_entry *entry,
                                 void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_traversal_step *, layer,
        entry->value);
    KEFIR_FREE(mem, layer);
    return KEFIR_OK;
}

static kefir_result_t push_layer(struct kefir_mem *mem,
                               struct kefir_ast_type_traversal *traversal,
                               const struct kefir_ast_type *object_type) {
    struct kefir_ast_type_traversal_step *layer = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_traversal_step));
    REQUIRE(layer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST type traversal layer"));
    layer->object_type = object_type;
    switch (object_type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE: {
            layer->type = KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE;
            layer->structure.iterator = kefir_list_head(&object_type->structure_type.fields);
            if (layer->structure.iterator == NULL) {
                KEFIR_FREE(mem, layer);
                return KEFIR_OK;
            }
        } break;

        case KEFIR_AST_TYPE_UNION: {
            layer->type = KEFIR_AST_TYPE_TRAVERSAL_UNION;
            layer->structure.iterator = kefir_list_head(&object_type->structure_type.fields);
            if (layer->structure.iterator == NULL) {
                KEFIR_FREE(mem, layer);
                return KEFIR_OK;
            }
        } break;

        case KEFIR_AST_TYPE_ARRAY: {
            REQUIRE(object_type->array_type.boundary != KEFIR_AST_ARRAY_VLA &&
                object_type->array_type.boundary != KEFIR_AST_ARRAY_VLA_STATIC,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot traverse VLA AST array types"));
            layer->type = KEFIR_AST_TYPE_TRAVERSAL_ARRAY;
            layer->array.index = 0;
        } break;
        
        default:
            layer->type = KEFIR_AST_TYPE_TRAVERSAL_SCALAR;
            break;
    }
    kefir_result_t res = kefir_linked_stack_push(mem, &traversal->stack, (void *) layer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, layer);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_init(struct kefir_mem *mem,
                                         struct kefir_ast_type_traversal *traversal,
                                         const struct kefir_ast_type *object_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    REQUIRE(object_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid object type"));

    traversal->current_object_type = object_type;
    REQUIRE_OK(kefir_list_init(&traversal->stack));
    REQUIRE_OK(kefir_list_on_remove(&traversal->stack, remove_layer, NULL));
    REQUIRE_OK(push_layer(mem, traversal, object_type));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_free(struct kefir_mem *mem,
                                         struct kefir_ast_type_traversal *traversal) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    REQUIRE_OK(kefir_list_free(mem, &traversal->stack));
    traversal->current_object_type = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_next(struct kefir_mem *mem,
                                         struct kefir_ast_type_traversal *traversal,
                                         const struct kefir_ast_type **type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type pointer"));
    
    if (kefir_list_length(&traversal->stack) == 0) {
        *type = NULL;
        return KEFIR_OK;
    }

    struct kefir_ast_type_traversal_step *layer = NULL;
    REQUIRE_OK(kefir_linked_stack_peek(&traversal->stack, (void **) &layer));
    switch (layer->type) {
        case KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE: {
            struct kefir_ast_struct_field *field = layer->structure.iterator->value;
            *type = field->type;
            kefir_list_next(&layer->structure.iterator);
            if (layer->structure.iterator == NULL) {
                REQUIRE_OK(kefir_linked_stack_pop(mem, &traversal->stack, NULL));
            }
        } break;

        case KEFIR_AST_TYPE_TRAVERSAL_UNION: {
            struct kefir_ast_struct_field *field = layer->structure.iterator->value;
            *type = field->type;
            REQUIRE_OK(kefir_linked_stack_pop(mem, &traversal->stack, NULL));
        } break;

        case KEFIR_AST_TYPE_TRAVERSAL_ARRAY: {
            *type = layer->object_type->array_type.element_type;
            layer->array.index++;
            if (layer->object_type->array_type.boundary != KEFIR_AST_ARRAY_UNBOUNDED &&
                layer->array.index >= layer->object_type->array_type.length &&
                layer->object_type->array_type.length > 0) {
                REQUIRE_OK(kefir_linked_stack_pop(mem, &traversal->stack, NULL));
            }
        } break;

        case KEFIR_AST_TYPE_TRAVERSAL_SCALAR:
            *type = layer->object_type;
            REQUIRE_OK(kefir_linked_stack_pop(mem, &traversal->stack, NULL));
            break;
    }

    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_next_recursive(struct kefir_mem *mem,
                                                   struct kefir_ast_type_traversal *traversal,
                                                   const struct kefir_ast_type **type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type pointer"));

    const struct kefir_ast_type *top_type = NULL;
    REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &top_type));
    if (top_type != NULL) {
        switch (top_type->tag) {
            case KEFIR_AST_TYPE_STRUCTURE:
            case KEFIR_AST_TYPE_UNION:
            case KEFIR_AST_TYPE_ARRAY:
                REQUIRE_OK(push_layer(mem, traversal, top_type));
                return kefir_ast_type_traversal_next_recursive(mem, traversal, type);

            default:
                *type = top_type;
                break;
        }
    } else {
        *type = NULL;
    }
    return KEFIR_OK;
}

static kefir_result_t navigate_member(struct kefir_mem *mem,
                                    struct kefir_ast_type_traversal *traversal,
                                    const char *member,
                                    kefir_bool_t push) {
    struct kefir_ast_type_traversal_step *layer = NULL;
    REQUIRE_OK(kefir_linked_stack_peek(&traversal->stack, (void **) &layer));
    switch (layer->type) {
        case KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE:
        case KEFIR_AST_TYPE_TRAVERSAL_UNION: {
            for (const struct kefir_list_entry *iter = kefir_list_head(&layer->object_type->structure_type.fields);
                iter != NULL;
                kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_struct_field *, field,
                    iter->value);
                if (strcmp(member, field->identifier) == 0) {
                    if (push) {
                        kefir_list_next(&iter);
                        if (iter == NULL) {
                            REQUIRE_OK(kefir_linked_stack_pop(mem, &traversal->stack, NULL));
                        } else {
                            layer->structure.iterator = iter;
                        }
                        REQUIRE_OK(push_layer(mem, traversal, field->type));
                    } else {
                        layer->structure.iterator = iter;
                    }
                    return KEFIR_OK;
                }
            }
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to find specified structure/union member");
        };

        case KEFIR_AST_TYPE_TRAVERSAL_ARRAY:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Member access is not supported for array types");

        case KEFIR_AST_TYPE_TRAVERSAL_SCALAR:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Member access is not supported for scalar types");
    }
    return KEFIR_OK;
}

static kefir_result_t navigate_index(struct kefir_mem *mem,
                                    struct kefir_ast_type_traversal *traversal,
                                    kefir_size_t index,
                                    kefir_bool_t push) {
    struct kefir_ast_type_traversal_step *layer = NULL;
    REQUIRE_OK(kefir_linked_stack_peek(&traversal->stack, (void **) &layer));
    switch (layer->type) {
        case KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE:
        case KEFIR_AST_TYPE_TRAVERSAL_UNION:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Indexed access is not supported for structure/union types");

        case KEFIR_AST_TYPE_TRAVERSAL_ARRAY: {
            const struct kefir_ast_type *array = layer->object_type;
            if (array->array_type.boundary != KEFIR_AST_ARRAY_UNBOUNDED &&
                array->array_type.length <= index) {
                return KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Specified index exceeds array bounds");
            }
            if (push) {
                index++;
                if (layer->object_type->array_type.boundary != KEFIR_AST_ARRAY_UNBOUNDED &&
                    index >= layer->object_type->array_type.length &&
                    layer->object_type->array_type.length > 0) {
                    REQUIRE_OK(kefir_linked_stack_pop(mem, &traversal->stack, NULL));
                } else {
                    layer->array.index = index;
                }
                REQUIRE_OK(push_layer(mem, traversal, array->array_type.element_type));
            } else {
                layer->array.index = index;
            }
        } break;

        case KEFIR_AST_TYPE_TRAVERSAL_SCALAR:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Indexed access is not supported for scalar types");
    }
    return KEFIR_OK;
}

static kefir_result_t navigate_impl(struct kefir_mem *mem,
                                  struct kefir_ast_type_traversal *traversal,
                                  const struct kefir_ast_designator *designator,
                                  kefir_bool_t push) {
    if (designator->next != NULL) {
        REQUIRE_OK(navigate_impl(mem, traversal, designator->next, true));
    }
    switch (designator->type) {
        case KEFIR_AST_DESIGNATOR_MEMBER:
            REQUIRE_OK(navigate_member(mem, traversal, designator->member, push));
            break;

        case KEFIR_AST_DESIGNATOR_SUBSCRIPT:
            REQUIRE_OK(navigate_index(mem, traversal, designator->index, push));
            break;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_navigate(struct kefir_mem *mem,
                                             struct kefir_ast_type_traversal *traversal,
                                             const struct kefir_ast_designator *designator) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST designator"));

    REQUIRE_OK(kefir_list_clear(mem, &traversal->stack));
    REQUIRE_OK(push_layer(mem, traversal, traversal->current_object_type));
    REQUIRE_OK(navigate_impl(mem, traversal, designator, false));
    return KEFIR_OK;
}

kefir_bool_t kefir_ast_type_traversal_empty(struct kefir_ast_type_traversal *traversal) {
    REQUIRE(traversal != NULL, true);
    return kefir_list_length(&traversal->stack) == 0;
}