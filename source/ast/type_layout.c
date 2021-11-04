/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "kefir/ast/type_layout.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

static kefir_result_t on_structure_member_remove(struct kefir_mem *mem, struct kefir_list *list,
                                                 struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_layout_structure_member *, member, entry->value);
    REQUIRE(member != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type layout member"));
    REQUIRE_OK(kefir_ast_type_layout_free(mem, member->layout));
    KEFIR_FREE(mem, member);
    return KEFIR_OK;
}

static kefir_result_t on_sublayout_remove(struct kefir_mem *mem, struct kefir_list *list,
                                          struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, sublayout, entry->value);
    REQUIRE(sublayout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type layout"));
    REQUIRE_OK(kefir_ast_type_layout_free(mem, sublayout));
    return KEFIR_OK;
}

struct kefir_ast_type_layout *kefir_ast_new_type_layout(struct kefir_mem *mem, const struct kefir_ast_type *type,
                                                        kefir_size_t alignment, kefir_uptr_t value) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_type_layout *layout = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_layout));
    REQUIRE(layout != NULL, NULL);
    layout->parent = NULL;
    layout->type = type;
    layout->alignment = alignment;
    layout->value = value;
    layout->properties.valid = false;
    if (type == NULL) {
        kefir_result_t res = kefir_list_init(&layout->custom_layout.sublayouts);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, layout);
            return NULL;
        });

        res = kefir_list_on_remove(&layout->custom_layout.sublayouts, on_sublayout_remove, NULL);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_list_free(mem, &layout->custom_layout.sublayouts);
            KEFIR_FREE(mem, layout);
            return NULL;
        });
    } else {
        switch (type->tag) {
            case KEFIR_AST_TYPE_STRUCTURE:
            case KEFIR_AST_TYPE_UNION: {
                kefir_result_t res = kefir_list_init(&layout->structure_layout.member_list);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    KEFIR_FREE(mem, layout);
                    return NULL;
                });
                res = kefir_list_on_remove(&layout->structure_layout.member_list, on_structure_member_remove, NULL);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    kefir_list_free(mem, &layout->structure_layout.member_list);
                    KEFIR_FREE(mem, layout);
                    return NULL;
                });
                res = kefir_hashtree_init(&layout->structure_layout.named_members, &kefir_hashtree_str_ops);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    kefir_list_free(mem, &layout->structure_layout.member_list);
                    KEFIR_FREE(mem, layout);
                    return NULL;
                });
            } break;

            case KEFIR_AST_TYPE_ARRAY:
                if (KEFIR_AST_TYPE_IS_VL_ARRAY(type)) {
                    layout->vl_array.array_ptr_field = 0;
                    layout->vl_array.array_size_field = 0;
                } else {
                    layout->array_layout.element_type = NULL;
                }
                break;

            default:
                break;
        }
    }
    return layout;
}

kefir_result_t kefir_ast_type_layout_free(struct kefir_mem *mem, struct kefir_ast_type_layout *type_layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(type_layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type layout"));

    if (type_layout->type == NULL) {
        REQUIRE_OK(kefir_list_free(mem, &type_layout->custom_layout.sublayouts));
    } else {
        switch (type_layout->type->tag) {
            case KEFIR_AST_TYPE_STRUCTURE:
            case KEFIR_AST_TYPE_UNION:
                REQUIRE_OK(kefir_hashtree_free(mem, &type_layout->structure_layout.named_members));
                REQUIRE_OK(kefir_list_free(mem, &type_layout->structure_layout.member_list));
                break;

            case KEFIR_AST_TYPE_ARRAY:
                if (!KEFIR_AST_TYPE_IS_VL_ARRAY(type_layout->type)) {
                    REQUIRE_OK(kefir_ast_type_layout_free(mem, type_layout->array_layout.element_type));
                }
                break;

            default:
                break;
        }
    }
    KEFIR_FREE(mem, type_layout);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_layout_insert_structure_member(struct kefir_mem *mem,
                                                             struct kefir_ast_type_layout *root_layout,
                                                             const char *identifier,
                                                             struct kefir_ast_type_layout *member) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(root_layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid root AST type layout"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid member AST type layout"));
    REQUIRE(member != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid member AST type layout"));
    REQUIRE(root_layout->type != NULL &&
                (root_layout->type->tag == KEFIR_AST_TYPE_STRUCTURE || root_layout->type->tag == KEFIR_AST_TYPE_UNION),
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Root AST type layout should have struct/union type"));

    struct kefir_ast_type_layout_structure_member *struct_member =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_layout_structure_member));
    REQUIRE(struct_member != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate structure member layout"));
    struct_member->identifier = identifier;
    struct_member->layout = member;

    kefir_result_t res = kefir_hashtree_insert(mem, &root_layout->structure_layout.named_members,
                                               (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) member);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, struct_member);
        return res;
    });

    res = kefir_list_insert_after(mem, &root_layout->structure_layout.member_list,
                                  kefir_list_tail(&root_layout->structure_layout.member_list), struct_member);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_delete(mem, &root_layout->structure_layout.named_members, (kefir_hashtree_key_t) identifier);
        KEFIR_FREE(mem, struct_member);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_layout_add_structure_anonymous_member(struct kefir_mem *mem,
                                                                    struct kefir_ast_type_layout *root_layout,
                                                                    struct kefir_ast_type_layout *member) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(root_layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid root AST type layout"));
    REQUIRE(member != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid member AST type layout"));
    REQUIRE(root_layout->type != NULL &&
                (root_layout->type->tag == KEFIR_AST_TYPE_STRUCTURE || root_layout->type->tag == KEFIR_AST_TYPE_UNION),
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Root AST type layout should have struct/union type"));

    struct kefir_ast_type_layout_structure_member *struct_member =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_layout_structure_member));
    REQUIRE(struct_member != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate structure member layout"));
    struct_member->identifier = NULL;
    struct_member->layout = member;

    kefir_result_t res =
        kefir_list_insert_after(mem, &root_layout->structure_layout.member_list,
                                kefir_list_tail(&root_layout->structure_layout.member_list), struct_member);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, struct_member);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t resolve_member(struct kefir_ast_type_layout *current_layout,
                                     const struct kefir_ast_designator *designator,
                                     struct kefir_ast_type_layout **layout,
                                     kefir_ast_type_layout_resolver_callback_t callback, void *payload) {
    REQUIRE(current_layout->type != NULL && (current_layout->type->tag == KEFIR_AST_TYPE_STRUCTURE ||
                                             current_layout->type->tag == KEFIR_AST_TYPE_UNION),
            KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Expected struct/union type to correspond to member designator"));

    struct kefir_hashtree_node *node = NULL;
    kefir_result_t res = kefir_hashtree_at(&current_layout->structure_layout.named_members,
                                           (kefir_hashtree_key_t) designator->member, &node);
    if (res == KEFIR_OK) {
        ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, next_layout, node->value);
        if (callback != NULL) {
            REQUIRE_OK(callback(next_layout, designator, payload));
        }
        *layout = next_layout;
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        for (const struct kefir_list_entry *iter = kefir_list_head(&current_layout->structure_layout.member_list);
             iter != NULL && res == KEFIR_NOT_FOUND; kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_type_layout_structure_member *, member, iter->value);
            if (member->identifier == NULL) {
                res = resolve_member(member->layout, designator, layout, callback, payload);
                if (res == KEFIR_OK && callback != NULL) {
                    REQUIRE_OK(callback(member->layout, designator, payload));
                }
            }
        }
    }
    return res;
}

static kefir_result_t resolve_subscript(struct kefir_ast_type_layout *current_layout,
                                        const struct kefir_ast_designator *designator,
                                        struct kefir_ast_type_layout **layout,
                                        kefir_ast_type_layout_resolver_callback_t callback, void *payload) {
    REQUIRE(current_layout->type != NULL && current_layout->type->tag == KEFIR_AST_TYPE_ARRAY,
            KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Expected array type to resolve subscript"));
    struct kefir_ast_type_layout *next_layout = current_layout->array_layout.element_type;
    *layout = next_layout;
    if (callback != NULL) {
        REQUIRE_OK(callback(next_layout, designator, payload));
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_layout(struct kefir_ast_type_layout *root, const struct kefir_ast_designator *designator,
                                     struct kefir_ast_type_layout **layout,
                                     kefir_ast_type_layout_resolver_callback_t callback, void *payload) {
    struct kefir_ast_type_layout *current_layout = NULL;
    if (designator->next != NULL) {
        REQUIRE_OK(resolve_layout(root, designator->next, &current_layout, callback, payload));
    } else {
        current_layout = root;
    }

    if (designator->type == KEFIR_AST_DESIGNATOR_MEMBER) {
        REQUIRE_OK(resolve_member(current_layout, designator, layout, callback, payload));
    } else {
        REQUIRE_OK(resolve_subscript(current_layout, designator, layout, callback, payload));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_layout_resolve(struct kefir_ast_type_layout *root,
                                             const struct kefir_ast_designator *designator,
                                             struct kefir_ast_type_layout **layout_ptr,
                                             kefir_ast_type_layout_resolver_callback_t callback, void *payload) {
    REQUIRE(root != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid root AST type layout"));
    REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST designator"));
    REQUIRE(layout_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type layout pointer"));

    REQUIRE_OK(resolve_layout(root, designator, layout_ptr, callback, payload));
    return KEFIR_OK;
}

static kefir_result_t add_to_offset(struct kefir_ast_type_layout *layout, const struct kefir_ast_designator *designator,
                                    void *payload) {
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST type layout"));
    REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid designator"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(kefir_size_t *, offset, payload);
    if (designator->type == KEFIR_AST_DESIGNATOR_MEMBER) {
        *offset += layout->properties.relative_offset;
    } else if (designator->type == KEFIR_AST_DESIGNATOR_SUBSCRIPT) {
        *offset += layout->properties.relative_offset + designator->index * layout->properties.size;
    } else {
        return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected designator type");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_layout_resolve_offset(struct kefir_ast_type_layout *root,
                                                    const struct kefir_ast_designator *designator,
                                                    struct kefir_ast_type_layout **layout_ptr, kefir_size_t *offset) {
    REQUIRE(root != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid root AST type layout"));
    REQUIRE(layout_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type layout pointer"));
    REQUIRE(offset != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid type layout offset pointer"));

    if (designator != NULL) {
        *offset = 0;
        REQUIRE_OK(kefir_ast_type_layout_resolve(root, designator, layout_ptr, add_to_offset, offset));
    } else {
        *layout_ptr = root;
        *offset = 0;
    }
    return KEFIR_OK;
}
