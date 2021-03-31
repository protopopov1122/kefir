#include <string.h>
#include "kefir/ast/type_layout.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t on_member_remove(struct kefir_mem *mem,
                                     struct kefir_hashtree *tree,
                                     kefir_hashtree_key_t key,
                                     kefir_hashtree_value_t value,
                                     void *payload) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, member,
        value);
    REQUIRE(member != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type layout member"));
    REQUIRE_OK(kefir_ast_type_layout_free(mem, member));
    return KEFIR_OK;
}

static kefir_result_t on_anonymous_member_remove(struct kefir_mem *mem,
                                               struct kefir_list *list,
                                               struct kefir_list_entry *entry,
                                               void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, member,
        entry->value);
    REQUIRE(member != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type layout member"));
    REQUIRE_OK(kefir_ast_type_layout_free(mem, member));
    return KEFIR_OK;
}

struct kefir_ast_type_layout *kefir_ast_new_type_layout(struct kefir_mem *mem,
                                                    const struct kefir_ast_type *type,
                                                    kefir_uptr_t value) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);
    
    struct kefir_ast_type_layout *layout = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_layout));
    REQUIRE(layout != NULL, NULL);
    layout->type = type;
    layout->value = value;
    layout->properties.valid = false;
    switch (type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION: {
            kefir_result_t res = kefir_hashtree_init(&layout->structure_layout.members, &kefir_hashtree_str_ops);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_FREE(mem, layout);
                return NULL;
            });
            res = kefir_hashtree_on_removal(&layout->structure_layout.members, on_member_remove, NULL);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_hashtree_free(mem, &layout->structure_layout.members);
                KEFIR_FREE(mem, layout);
                return NULL;
            });

            res = kefir_list_init(&layout->structure_layout.anonymous_members);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_hashtree_free(mem, &layout->structure_layout.members);
                KEFIR_FREE(mem, layout);
                return NULL;
            });
            res = kefir_list_on_remove(&layout->structure_layout.anonymous_members, on_anonymous_member_remove, NULL);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_list_free(mem, &layout->structure_layout.anonymous_members);
                kefir_hashtree_free(mem, &layout->structure_layout.members);
                KEFIR_FREE(mem, layout);
                return NULL;
            });
        } break;

        case KEFIR_AST_TYPE_ARRAY:
            layout->array_layout.element_type = NULL;
            break;

        default:
            break;
    }
    return layout;
}

kefir_result_t kefir_ast_type_layout_free(struct kefir_mem *mem,
                                      struct kefir_ast_type_layout *type_layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type layout"));
    
    switch (type_layout->type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
            REQUIRE_OK(kefir_list_free(mem, &type_layout->structure_layout.anonymous_members));
            REQUIRE_OK(kefir_hashtree_free(mem, &type_layout->structure_layout.members));
            break;

        case KEFIR_AST_TYPE_ARRAY:
            REQUIRE_OK(kefir_ast_type_layout_free(mem, type_layout->array_layout.element_type));
            break;

        default:
            break;
    }
    KEFIR_FREE(mem, type_layout);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_layout_insert_structure_member(struct kefir_mem *mem,
                                                         struct kefir_ast_type_layout *root_layout,
                                                         const char *identifier,
                                                         struct kefir_ast_type_layout *member) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(root_layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid root AST type layout"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid member AST type layout"));
    REQUIRE(member != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid member AST type layout"));
    REQUIRE(root_layout->type->tag == KEFIR_AST_TYPE_STRUCTURE ||
        root_layout->type->tag == KEFIR_AST_TYPE_UNION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Root AST type layout should have struct/union type"));

    REQUIRE_OK(kefir_hashtree_insert(mem, &root_layout->structure_layout.members,
        (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) member));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_layout_add_structure_anonymous_member(struct kefir_mem *mem,
                                                                struct kefir_ast_type_layout *root_layout,
                                                                struct kefir_ast_type_layout *member) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(root_layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid root AST type layout"));
    REQUIRE(member != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid member AST type layout"));
    REQUIRE(root_layout->type->tag == KEFIR_AST_TYPE_STRUCTURE ||
        root_layout->type->tag == KEFIR_AST_TYPE_UNION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Root AST type layout should have struct/union type"));

    REQUIRE_OK(kefir_list_insert_after(mem, &root_layout->structure_layout.anonymous_members,
        kefir_list_tail(&root_layout->structure_layout.anonymous_members), member));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_layout_resolve_member(const struct kefir_ast_type_layout *root_layout,
                                                const char *identifier,
                                                const struct kefir_ast_type_layout **layout) {
    REQUIRE(root_layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid root AST type layout"));
    REQUIRE(root_layout->type->tag == KEFIR_AST_TYPE_STRUCTURE ||
        root_layout->type->tag == KEFIR_AST_TYPE_UNION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Root AST type layout should have struct/union type"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid member AST type layout"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type layout pointer"));

    struct kefir_hashtree_node *node = NULL;
    kefir_result_t res = kefir_hashtree_at(&root_layout->structure_layout.members,
        (kefir_hashtree_key_t) identifier, &node);
    if (res == KEFIR_OK) {
        *layout = (const struct kefir_ast_type_layout *) node->value;
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        for (const struct kefir_list_entry *iter = kefir_list_head(&root_layout->structure_layout.anonymous_members);
            iter != NULL && res == KEFIR_NOT_FOUND;
            kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(const struct kefir_ast_type_layout *, anon_member,
                iter->value);
            res = kefir_ast_type_layout_resolve_member(anon_member, identifier, layout);
        }
    }
    return res;
}