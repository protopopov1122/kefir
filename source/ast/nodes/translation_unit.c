#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_translation_unit_visit, kefir_ast_translation_unit, translation_unit)

struct kefir_ast_node_base *ast_translation_unit_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_translation_unit_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_translation_unit *, node, base->self);

    REQUIRE_OK(kefir_list_free(mem, &node->external_definitions));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_TRANSLATION_UNIT_CLASS = {.type = KEFIR_AST_TRANSLATION_UNIT,
                                                                .visit = ast_translation_unit_visit,
                                                                .clone = ast_translation_unit_clone,
                                                                .free = ast_translation_unit_free};

static kefir_result_t external_definition_free(struct kefir_mem *mem, struct kefir_list *list,
                                               struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, ext_def, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, ext_def));
    return KEFIR_OK;
}

struct kefir_ast_node_base *ast_translation_unit_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_translation_unit *, node, base->self);
    struct kefir_ast_translation_unit *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translation_unit));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_TRANSLATION_UNIT_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_list_init(&clone->external_definitions);
    REQUIRE_CHAIN(&res, kefir_list_on_remove(&clone->external_definitions, external_definition_free, NULL));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->external_definitions); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, ext_def, iter->value);

        struct kefir_ast_node_base *clone_ext_def = KEFIR_AST_NODE_CLONE(mem, ext_def);
        REQUIRE_ELSE(clone_ext_def != NULL, {
            kefir_list_free(mem, &clone->external_definitions);
            KEFIR_FREE(mem, clone);
            return NULL;
        });

        res = kefir_list_insert_after(mem, &clone->external_definitions, kefir_list_tail(&clone->external_definitions),
                                      clone_ext_def);
        REQUIRE_ELSE(clone_ext_def != NULL, {
            KEFIR_AST_NODE_FREE(mem, clone_ext_def);
            kefir_list_free(mem, &clone->external_definitions);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    }

    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_translation_unit *kefir_ast_new_translation_unit(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_translation_unit *unit = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translation_unit));
    REQUIRE(unit != NULL, NULL);
    unit->base.klass = &AST_TRANSLATION_UNIT_CLASS;
    unit->base.self = unit;
    kefir_result_t res = kefir_ast_node_properties_init(&unit->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, unit);
        return NULL;
    });

    res = kefir_list_init(&unit->external_definitions);
    REQUIRE_CHAIN(&res, kefir_list_on_remove(&unit->external_definitions, external_definition_free, NULL));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, unit);
        return NULL;
    });

    return unit;
}
