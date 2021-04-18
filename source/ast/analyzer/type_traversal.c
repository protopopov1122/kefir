#include "kefir/ast/analyzer/type_traversal.h"
#include "kefir/core/linked_stack.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_bool_t is_array_finished(const struct kefir_ast_type *type,
                                    kefir_size_t index) {
    return type->array_type.boundary != KEFIR_AST_ARRAY_UNBOUNDED &&
           index >= kefir_ast_type_array_const_length(&type->array_type) &&
           kefir_ast_type_array_const_length(&type->array_type) > 0;
}

static kefir_result_t remove_layer(struct kefir_mem *mem,
                                 struct kefir_list *list,
                                 struct kefir_list_entry *entry,
                                 void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_traversal_layer *, layer,
        entry->value);
    KEFIR_FREE(mem, layer);
    return KEFIR_OK;
}

static kefir_result_t push_layer(struct kefir_mem *mem,
                               struct kefir_ast_type_traversal *traversal,
                               const struct kefir_ast_type *object_type,
                               const struct kefir_ast_type_traversal_layer *parent) {
    struct kefir_ast_type_traversal_layer *layer = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_traversal_layer));
    REQUIRE(layer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST type traversal layer"));
    layer->parent = parent;
    layer->object_type = object_type;
    layer->init = true;
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
    if (traversal->events.layer_begin != NULL) {
        REQUIRE_OK(traversal->events.layer_begin(traversal, layer, traversal->events.payload));
    }
    return KEFIR_OK;
}

static kefir_result_t pop_layer(struct kefir_mem *mem,
                              struct kefir_ast_type_traversal *traversal) {
    if (kefir_list_length(&traversal->stack) > 0) {
        if (traversal->events.layer_end != NULL) {
            struct kefir_ast_type_traversal_layer *layer = NULL;
            REQUIRE_OK(kefir_linked_stack_peek(&traversal->stack, (void **) &layer));
            REQUIRE_OK(traversal->events.layer_end(traversal, layer, traversal->events.payload));
        }
        REQUIRE_OK(kefir_linked_stack_pop(mem, &traversal->stack, NULL));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_init(struct kefir_mem *mem,
                                         struct kefir_ast_type_traversal *traversal,
                                         const struct kefir_ast_type *object_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    REQUIRE(object_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid object type"));

    traversal->current_object_type = object_type;
    traversal->events = (struct kefir_ast_type_traversal_events){0};
    REQUIRE_OK(kefir_list_init(&traversal->stack));
    REQUIRE_OK(kefir_list_on_remove(&traversal->stack, remove_layer, NULL));
    REQUIRE_OK(push_layer(mem, traversal, object_type, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_free(struct kefir_mem *mem,
                                         struct kefir_ast_type_traversal *traversal) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    while (kefir_list_length(&traversal->stack) > 0) {
        REQUIRE_OK(pop_layer(mem, traversal));
    }
    REQUIRE_OK(kefir_list_free(mem, &traversal->stack));
    traversal->current_object_type = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_next(struct kefir_mem *mem,
                                         struct kefir_ast_type_traversal *traversal,
                                         const struct kefir_ast_type **type,
                                         const struct kefir_ast_type_traversal_layer **layer_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type pointer"));
    
    if (kefir_list_length(&traversal->stack) == 0) {
        ASSIGN_PTR(layer_ptr, NULL);
        *type = NULL;
        return KEFIR_OK;
    }

    struct kefir_ast_type_traversal_layer *layer = NULL;
    REQUIRE_OK(kefir_linked_stack_peek(&traversal->stack, (void **) &layer));
    switch (layer->type) {
        case KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE: {
            if (!layer->init) {
                kefir_list_next(&layer->structure.iterator);
            }
            if (layer->structure.iterator == NULL) {
                REQUIRE_OK(pop_layer(mem, traversal));
                return kefir_ast_type_traversal_next(mem, traversal, type, layer_ptr);
            }
            layer->init = false;
            struct kefir_ast_struct_field *field = layer->structure.iterator->value;
            *type = field->type;
            ASSIGN_PTR(layer_ptr, layer);
        } break;

        case KEFIR_AST_TYPE_TRAVERSAL_UNION: {
            if (!layer->init || layer->structure.iterator == NULL) {
                REQUIRE_OK(pop_layer(mem, traversal));
                return kefir_ast_type_traversal_next(mem, traversal, type, layer_ptr);
            }
            layer->init = false;
            struct kefir_ast_struct_field *field = layer->structure.iterator->value;
            *type = field->type;
            ASSIGN_PTR(layer_ptr, layer);
        } break;

        case KEFIR_AST_TYPE_TRAVERSAL_ARRAY: {
            if (!layer->init) {
                layer->array.index++;
            }
            if (is_array_finished(layer->object_type, layer->array.index)) {
                REQUIRE_OK(pop_layer(mem, traversal));
                return kefir_ast_type_traversal_next(mem, traversal, type, layer_ptr);
            }
            layer->init = false;
            *type = layer->object_type->array_type.element_type;
            ASSIGN_PTR(layer_ptr, layer);
        } break;

        case KEFIR_AST_TYPE_TRAVERSAL_SCALAR:
            if (!layer->init) {
                REQUIRE_OK(pop_layer(mem, traversal));
                return kefir_ast_type_traversal_next(mem, traversal, type, layer_ptr);
            }
            layer->init = false;
            *type = layer->object_type;
            ASSIGN_PTR(layer_ptr, layer);
            break;
    }

    if (traversal->events.layer_next != NULL) {
        REQUIRE_OK(traversal->events.layer_next(traversal, layer, traversal->events.payload));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_next_recursive(struct kefir_mem *mem,
                                                   struct kefir_ast_type_traversal *traversal,
                                                   const struct kefir_ast_type **type,
                                                   const struct kefir_ast_type_traversal_layer **layer_ptr) {
    return kefir_ast_type_traversal_next_recursive2(mem, traversal, NULL, NULL, type, layer_ptr);
}

kefir_result_t kefir_ast_type_traversal_next_recursive2(struct kefir_mem *mem,
                                                    struct kefir_ast_type_traversal *traversal,
                                                    kefir_bool_t (*stop)(const struct kefir_ast_type *, void *),
                                                    void *stop_payload,
                                                    const struct kefir_ast_type **type,
                                                    const struct kefir_ast_type_traversal_layer **layer_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type pointer"));

    const struct kefir_ast_type *top_type = NULL;
    const struct kefir_ast_type_traversal_layer *top_layer = NULL;
    REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &top_type, &top_layer));
    if (top_type != NULL) {
        switch (top_type->tag) {
            case KEFIR_AST_TYPE_STRUCTURE:
            case KEFIR_AST_TYPE_UNION:
            case KEFIR_AST_TYPE_ARRAY:
                if (stop == NULL || !stop(top_type, stop_payload)) {
                    REQUIRE_OK(push_layer(mem, traversal, top_type, top_layer));
                    return kefir_ast_type_traversal_next_recursive2(mem, traversal, stop, stop_payload, type, layer_ptr);
                }
                // Intentional fallthrough

            default:
                ASSIGN_PTR(layer_ptr, top_layer);
                *type = top_type;
                break;
        }
    } else {
        ASSIGN_PTR(layer_ptr, NULL);
        *type = NULL;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_traversal_step(struct kefir_mem *mem,
                                         struct kefir_ast_type_traversal *traversal) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(traversal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal structure"));
    
    if (kefir_list_length(&traversal->stack) == 0) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Type traversal is empty");
    }

    struct kefir_ast_type_traversal_layer *layer = NULL;
    REQUIRE_OK(kefir_linked_stack_peek(&traversal->stack, (void **) &layer));
    switch (layer->type) {
        case KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE:
        case KEFIR_AST_TYPE_TRAVERSAL_UNION: {
            struct kefir_ast_struct_field *field = layer->structure.iterator->value;
            REQUIRE_OK(push_layer(mem, traversal, field->type, layer));
        } break;

        case KEFIR_AST_TYPE_TRAVERSAL_ARRAY:
            REQUIRE_OK(push_layer(mem, traversal, layer->object_type->array_type.element_type, layer));
            break;

        case KEFIR_AST_TYPE_TRAVERSAL_SCALAR:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Type traversal is unable to step into scalar");
    }
    return KEFIR_OK;
}

static kefir_result_t navigate_member(struct kefir_mem *mem,
                                    struct kefir_ast_type_traversal *traversal,
                                    const char *member,
                                    kefir_bool_t push) {
    struct kefir_ast_type_traversal_layer *layer = NULL;
    REQUIRE_OK(kefir_linked_stack_peek(&traversal->stack, (void **) &layer));
    switch (layer->type) {
        case KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE: {
            for (const struct kefir_list_entry *iter = kefir_list_head(&layer->object_type->structure_type.fields);
                iter != NULL;
                kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_struct_field *, field,
                    iter->value);
                if (strcmp(member, field->identifier) == 0) {
                    layer->init = false;
                    if (push) {
                        layer->structure.iterator = iter;
                        REQUIRE_OK(push_layer(mem, traversal, field->type, layer));
                    } else {
                        if (iter->prev != NULL) {
                            layer->structure.iterator = iter->prev;
                        } else {
                            layer->structure.iterator = iter;
                            layer->init = true;
                        }
                    }
                    return KEFIR_OK;
                }
            }
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to find specified structure/union member");
        };

        case KEFIR_AST_TYPE_TRAVERSAL_UNION: {
            for (const struct kefir_list_entry *iter = kefir_list_head(&layer->object_type->structure_type.fields);
                iter != NULL;
                kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_struct_field *, field,
                    iter->value);
                if (strcmp(member, field->identifier) == 0) {
                    layer->init = true;
                    layer->structure.iterator = iter;
                    if (push) {
                        REQUIRE_OK(push_layer(mem, traversal, field->type, layer));
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
    struct kefir_ast_type_traversal_layer *layer = NULL;
    REQUIRE_OK(kefir_linked_stack_peek(&traversal->stack, (void **) &layer));
    switch (layer->type) {
        case KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE:
        case KEFIR_AST_TYPE_TRAVERSAL_UNION:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Indexed access is not supported for structure/union types");

        case KEFIR_AST_TYPE_TRAVERSAL_ARRAY: {
            const struct kefir_ast_type *array = layer->object_type;
            if (array->array_type.boundary != KEFIR_AST_ARRAY_UNBOUNDED &&
                kefir_ast_type_array_const_length(&array->array_type) <= index) {
                return KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Specified index exceeds array bounds");
            }
            layer->init = false;
            if (push) {
                layer->array.index = index;
                REQUIRE_OK(push_layer(mem, traversal, array->array_type.element_type, layer));
            } else {
                if (index > 0) {
                    layer->array.index = index - 1;
                } else {
                    layer->init = true;
                    layer->array.index = 0;
                }
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
    REQUIRE_OK(push_layer(mem, traversal, traversal->current_object_type, NULL));
    REQUIRE_OK(navigate_impl(mem, traversal, designator, false));
    return KEFIR_OK;
}

kefir_bool_t kefir_ast_type_traversal_empty(struct kefir_ast_type_traversal *traversal) {
    REQUIRE(traversal != NULL, true);
    const struct kefir_list_entry *iter = kefir_list_head(&traversal->stack);
    for (; iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_type_traversal_layer *, step,
            iter->value);
        kefir_bool_t empty = true;
        switch (step->type) {
            case KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE:
                empty = step->structure.iterator == NULL ||
                    (!step->init && step->structure.iterator->next == NULL);
                break;

            case KEFIR_AST_TYPE_TRAVERSAL_UNION:
                empty = step->structure.iterator == NULL || !step->init;
                break;

            case KEFIR_AST_TYPE_TRAVERSAL_ARRAY:
                empty = is_array_finished(step->object_type, step->array.index) ||
                    (!step->init && is_array_finished(step->object_type, step->array.index + 1));
                break;

            case KEFIR_AST_TYPE_TRAVERSAL_SCALAR:
                empty = !step->init;
                break;
        }
        if (!empty) {
            return false;
        }
    }
    return true;
}

struct kefir_ast_designator *kefir_ast_type_traversal_layer_designator(struct kefir_mem *mem,
                                                                   struct kefir_symbol_table *symbols,
                                                                   const struct kefir_ast_type_traversal_layer *layer) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(layer != NULL, NULL);

    struct kefir_ast_designator *base = kefir_ast_type_traversal_layer_designator(mem, symbols, layer->parent);
    struct kefir_ast_designator *designator = NULL;
    switch (layer->type) {
        case KEFIR_AST_TYPE_TRAVERSAL_STRUCTURE:
        case KEFIR_AST_TYPE_TRAVERSAL_UNION: {
            ASSIGN_DECL_CAST(struct kefir_ast_struct_field *, field,
                layer->structure.iterator->value);
            if (field != NULL) {
                designator = kefir_ast_new_member_desginator(mem, symbols, field->identifier, base);
            }
        } break;

        case KEFIR_AST_TYPE_TRAVERSAL_ARRAY:
            designator = kefir_ast_new_index_desginator(mem, layer->array.index, base);
            break;

        case KEFIR_AST_TYPE_TRAVERSAL_SCALAR:
            // Intentionally left blank
            break;
    }
    return designator;
}