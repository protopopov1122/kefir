#include "kefir/ast/translator/layout.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/hashtree.h"

struct eval_param {
    struct kefir_mem *mem;
    const struct kefir_ast_translator_environment *env;
    kefir_ir_target_platform_opaque_type_t platform_type;
    struct kefir_hashtree *tree;
    struct kefir_ir_type_visitor *visitor;
};

static kefir_result_t traverse_layout(struct kefir_mem *mem,
                                    struct kefir_ast_type_layout *layout,
                                    struct eval_param *param) {
    REQUIRE_OK(kefir_hashtree_insert(mem, param->tree,
        (kefir_hashtree_key_t) layout->value, (kefir_hashtree_value_t) layout));
    switch (layout->type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION: {
            struct kefir_hashtree_node_iterator iter;
            for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&layout->structure_layout.members, &iter);
                node != NULL;
                node = kefir_hashtree_next(&iter)) {
                
                ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, member,
                    node->value);
                REQUIRE_OK(traverse_layout(mem, member, param));
            }

            for (const struct kefir_list_entry *iter2 = kefir_list_head(&layout->structure_layout.anonymous_members);
                iter2 != NULL;
                kefir_list_next(&iter2)) {
                ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, member,
                    iter2->value);
                REQUIRE_OK(traverse_layout(mem, member, param));
            }
        } break;

        case KEFIR_AST_TYPE_ARRAY:
            REQUIRE_OK(traverse_layout(mem, layout->array_layout.element_type, param));
            break;

        default:
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t type_visit(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               const struct kefir_ir_typeentry *typeentry,
                               void *payload) {
    ASSIGN_DECL_CAST(struct eval_param *, param,
        payload);
    
    struct kefir_ir_target_type_info type_info;
    REQUIRE_OK(KEFIR_IR_TARGET_PLATFORM_TYPE_INFO(param->mem, param->env->target_platform,
        param->platform_type, index, &type_info));

    struct kefir_hashtree_node *node = NULL;
    kefir_result_t res = kefir_hashtree_at(param->tree, (kefir_hashtree_key_t) index, &node);
    if (res == KEFIR_OK) {
        ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, layout,
            node->value);
        layout->properties.valid = true;
        layout->properties.size = type_info.size;
        layout->properties.alignment = type_info.alignment;
        layout->properties.aligned = type_info.aligned;
        layout->properties.relative_offset = type_info.relative_offset;
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
    }

    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_STRUCT:
        case KEFIR_IR_TYPE_UNION: {
            struct eval_param nested = {
                .mem = param->mem,
                .env = param->env,
                .platform_type = param->platform_type,
                .tree = param->tree,
                .visitor = param->visitor
            };
            REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, &nested, index + 1, typeentry->param));
        } break;

        case KEFIR_IR_TYPE_ARRAY: {
            struct eval_param nested = {
                .mem = param->mem,
                .env = param->env,
                .platform_type = param->platform_type,
                .tree = param->tree,
                .visitor = param->visitor
            };
            REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, &nested, index + 1, 1));
        } break;

        default:
            break;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_evaluate_type_layout(struct kefir_mem *mem,
                                                     const struct kefir_ast_translator_environment *env,
                                                     struct kefir_ast_type_layout *layout,
                                                     const struct kefir_ir_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type layout"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));

    struct kefir_ir_type_visitor visitor;
    struct kefir_hashtree tree;
    struct eval_param param = {
        .mem = mem,
        .env = env,
        .platform_type = NULL,
        .tree = &tree,
        .visitor = &visitor
    };

    REQUIRE_OK(kefir_hashtree_init(&tree, &kefir_hashtree_uint_ops));
    kefir_result_t res = traverse_layout(mem, layout, &param);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &tree);
        return res;
    });

    res = KEFIR_IR_TARGET_PLATFORM_GET_TYPE(mem, env->target_platform, type, &param.platform_type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &tree);
        return res;
    });

    res = kefir_ir_type_visitor_init(&visitor, type_visit);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IR_TARGET_PLATFORM_FREE_TYPE(mem, env->target_platform, param.platform_type);
        kefir_hashtree_free(mem, &tree);
        return res;
    });

    res = kefir_ir_type_visitor_list_nodes(type, &visitor, &param, 0, kefir_ir_type_total_length(type));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IR_TARGET_PLATFORM_FREE_TYPE(mem, env->target_platform, param.platform_type);
        kefir_hashtree_free(mem, &tree);
        return KEFIR_OK;
    });


    REQUIRE_OK(KEFIR_IR_TARGET_PLATFORM_FREE_TYPE(mem, env->target_platform, param.platform_type));
    REQUIRE_OK(kefir_hashtree_free(mem, &tree));
    return KEFIR_OK;
}