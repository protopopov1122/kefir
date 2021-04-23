#include "kefir/ast-translator/type_cache.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/layout.h"
#include "kefir/ir/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_hashtree_hash_t ast_type_hash(kefir_hashtree_key_t key, void *data) {
    UNUSED(data);
    ASSIGN_DECL_CAST(const struct kefir_ast_type *, type,
        key);
    REQUIRE(type != 0, 0);
    return (kefir_hashtree_hash_t) KEFIR_AST_TYPE_HASH(type);
}

static bool ast_type_compare(kefir_hashtree_key_t key1, kefir_hashtree_key_t key2, void *data) {
    UNUSED(data);
    ASSIGN_DECL_CAST(const struct kefir_ast_type *, type1,
        key1);
    ASSIGN_DECL_CAST(const struct kefir_ast_type *, type2,
        key2);
    return type1 == type2 || KEFIR_AST_TYPE_SAME(type1, type2);
}

const struct kefir_hashtree_ops ast_type_ops = {
    .hash = ast_type_hash,
    .compare_keys = ast_type_compare,
    .data = NULL
};

static kefir_result_t clone_cached_type(struct kefir_mem *mem,
                                      const struct kefir_ast_translator_cached_type *original,
                                      struct kefir_ast_type_layout *layout,
                                      struct kefir_ast_translator_cached_type **result) {
    *result = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_cached_type));
    REQUIRE(*result != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator cached type"));
    (*result)->klass = original->klass;
    switch (original->klass) {
        case KEFIR_AST_TRANSLATOR_CACHED_OBJECT_TYPE: {
            (*result)->object.ir_type_id = original->object.ir_type_id;
            (*result)->object.ir_type = original->object.ir_type;
            (*result)->object.type_layout = layout;
            (*result)->object.layout_owner = false;
        } break;

        case KEFIR_AST_TRANSLATOR_CACHED_FUNCTION_TYPE: {
            (*result)->function.declaration = original->function.declaration;
            (*result)->function.decl_owner = false;
        } break;
    }
    return KEFIR_OK;
}

static kefir_result_t free_cached_type(struct kefir_mem *mem,
                                     struct kefir_ast_translator_cached_type *cached_type) {
    switch (cached_type->klass) {
        case KEFIR_AST_TRANSLATOR_CACHED_OBJECT_TYPE: {
            if (cached_type->object.layout_owner && cached_type->object.type_layout != NULL) {
                REQUIRE_OK(kefir_ast_type_layout_free(mem, cached_type->object.type_layout));
            }
            cached_type->object.layout_owner = false;
            cached_type->object.type_layout = NULL;
            cached_type->object.ir_type = NULL;
            cached_type->object.ir_type_id = 0;
        } break;

        case KEFIR_AST_TRANSLATOR_CACHED_FUNCTION_TYPE: {
            if (cached_type->function.decl_owner) {
                REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, cached_type->function.declaration));
            }
            cached_type->function.decl_owner = false;
            cached_type->function.declaration = NULL;
        } break;
    }
    KEFIR_FREE(mem, cached_type);
    return KEFIR_OK;
}

static kefir_result_t aligned_cache_on_remove(struct kefir_mem *mem,
                                            struct kefir_hashtree *tree,
                                            kefir_hashtree_key_t key,
                                            kefir_hashtree_value_t value,
                                            void *payload) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_cached_type *, cached_type,
        value);
    REQUIRE(cached_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid cached type"));
    REQUIRE_OK(free_cached_type(mem, cached_type));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_aligned_type_cache_init(struct kefir_ast_translator_aligned_type_cache *cache,
                                                        kefir_size_t alignment) {
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator alignned type cache"));
    REQUIRE_OK(kefir_hashtree_init(&cache->cache, &ast_type_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&cache->cache, aligned_cache_on_remove, NULL));
    cache->alignment = alignment;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_aligned_type_cache_free(struct kefir_mem *mem,
                                                        struct kefir_ast_translator_aligned_type_cache *cache) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator alignned type cache"));
    REQUIRE_OK(kefir_hashtree_free(mem, &cache->cache));
    cache->alignment = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_aligned_type_cache_at(struct kefir_ast_translator_aligned_type_cache *cache,
                                                      const struct kefir_ast_type *type,
                                                      const struct kefir_ast_translator_cached_type **cached_type) {
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator alignned type cache"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(cached_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator cached type"));

    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&cache->cache, (kefir_hashtree_key_t) type, &node));
    *cached_type = (const struct kefir_ast_translator_cached_type *) node->value;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_aligned_type_layout_insert(struct kefir_mem *mem,
                                                           struct kefir_ast_translator_aligned_type_cache *cache,
                                                           struct kefir_ast_translator_cached_type *cached_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator alignned type cache"));
    REQUIRE(cached_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator cached type"));
    
    switch (cached_type->klass) {
        case KEFIR_AST_TRANSLATOR_CACHED_OBJECT_TYPE: {
            REQUIRE(cached_type->object.type_layout->alignment == cache->alignment,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Provided layout alignment does not correspond to cache"));
            
            REQUIRE_OK(kefir_hashtree_insert(mem, &cache->cache,
                (kefir_hashtree_key_t) cached_type->object.type_layout->type,
                (kefir_hashtree_value_t) cached_type));
        } break;

        case KEFIR_AST_TRANSLATOR_CACHED_FUNCTION_TYPE: {
            REQUIRE(cache->alignment == 0,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function type cannot be inserted into non-zero aligned cache"));

            REQUIRE_OK(kefir_hashtree_insert(mem, &cache->cache,
                (kefir_hashtree_key_t) cached_type->function.declaration->function_type,
                (kefir_hashtree_value_t) cached_type));
        } break;
    }
    return KEFIR_OK;
}

static kefir_result_t cache_on_remove(struct kefir_mem *mem,
                                    struct kefir_hashtree *tree,
                                    kefir_hashtree_key_t key,
                                    kefir_hashtree_value_t value,
                                    void *payload) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_aligned_type_cache *, aligned_cache,
        value);
    REQUIRE(aligned_cache != NULL,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type aligned cache"));
    REQUIRE_OK(kefir_ast_translator_aligned_type_cache_free(mem, aligned_cache));
    KEFIR_FREE(mem, aligned_cache);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_cache_init(struct kefir_ast_translator_type_cache *cache) {
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type cache"));
    REQUIRE_OK(kefir_hashtree_init(&cache->cache, &kefir_hashtree_uint_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&cache->cache, cache_on_remove, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_cache_free(struct kefir_mem *mem,
                                                struct kefir_ast_translator_type_cache *cache) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type cache"));
    REQUIRE_OK(kefir_hashtree_free(mem, &cache->cache));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_cache_at(struct kefir_ast_translator_type_cache *cache,
                                              const struct kefir_ast_type *type,
                                              kefir_size_t alignment,
                                              const struct kefir_ast_translator_cached_type **cached_type) {
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type cache"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(cached_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator cached type pointer"));

    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&cache->cache, (kefir_hashtree_key_t) alignment, &node));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_aligned_type_cache *, aligned_cache,
        node->value);
    REQUIRE_OK(kefir_ast_translator_aligned_type_cache_at(aligned_cache, type, cached_type));
    return KEFIR_OK;
}

static kefir_result_t traverse_object_type_layout(struct kefir_mem *mem,
                                                struct kefir_ast_translator_type_cache *cache,
                                                const struct kefir_ast_translator_cached_type *cached_type) {
    struct kefir_ast_translator_cached_type *cached_subtype = NULL;
    switch (cached_type->object.type_layout->type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION: {
            for (const struct kefir_list_entry *iter =
                    kefir_list_head(&cached_type->object.type_layout->structure_layout.member_list);
                iter != NULL;
                kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_type_layout_structure_member *, member,
                    iter->value);
                REQUIRE_OK(clone_cached_type(mem, cached_type, member->layout, &cached_subtype));
                kefir_result_t res = kefir_ast_translator_type_cache_insert(mem, cache, cached_subtype);
                if (res == KEFIR_ALREADY_EXISTS) {
                    free_cached_type(mem, cached_subtype);
                } else {
                    REQUIRE_OK(res);
                }
            }
        } break;

        case KEFIR_AST_TYPE_ARRAY: {
            REQUIRE_OK(clone_cached_type(mem, cached_type, 
                cached_type->object.type_layout->array_layout.element_type, &cached_subtype));
            kefir_result_t res = kefir_ast_translator_type_cache_insert(mem, cache, cached_subtype);
            if (res == KEFIR_ALREADY_EXISTS) {
                free_cached_type(mem, cached_subtype);
            } else {
                REQUIRE_OK(res);
            };
        } break;

        default:
            // Intentionally left blank
            break;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_cache_insert(struct kefir_mem *mem,
                                                 struct kefir_ast_translator_type_cache *cache,
                                                 struct kefir_ast_translator_cached_type *cached_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator alignned type cache"));
    REQUIRE(cached_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST cached type"));

    kefir_size_t alignment = 0;
    if (cached_type->klass == KEFIR_AST_TRANSLATOR_CACHED_OBJECT_TYPE) {
        alignment = cached_type->object.type_layout->alignment;
    }

    struct kefir_ast_translator_aligned_type_cache *aligned_cache = NULL;
    struct kefir_hashtree_node *aligned_cache_node = NULL;
    kefir_result_t res = kefir_hashtree_at(&cache->cache, (kefir_hashtree_key_t) alignment,
        &aligned_cache_node);
    if (res == KEFIR_NOT_FOUND) {
        aligned_cache = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_aligned_type_cache));
        REQUIRE(aligned_cache != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator type aligned cache"));
        res = kefir_ast_translator_aligned_type_cache_init(aligned_cache, alignment);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, aligned_cache);
            return res;
        });
        res = kefir_hashtree_insert(mem, &cache->cache,
            (kefir_hashtree_key_t) alignment, (kefir_hashtree_value_t) aligned_cache);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_translator_aligned_type_cache_free(mem, aligned_cache);
            KEFIR_FREE(mem, aligned_cache);
            return res;
        });
    } else {
        REQUIRE_OK(res);
        aligned_cache = (struct kefir_ast_translator_aligned_type_cache *) aligned_cache_node->value;
    }

    REQUIRE_OK(kefir_ast_translator_aligned_type_layout_insert(mem, aligned_cache, cached_type));
    if (cached_type->klass == KEFIR_AST_TRANSLATOR_CACHED_OBJECT_TYPE) {
        REQUIRE_OK(traverse_object_type_layout(mem, cache, cached_type));
    }
    return res;
}

kefir_result_t kefir_ast_translator_type_cache_clear(struct kefir_mem *mem,
                                                 struct kefir_ast_translator_type_cache *cache) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator alignned type cache"));
    REQUIRE_OK(kefir_hashtree_clean(mem, &cache->cache));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_cache_insert_unowned_object(struct kefir_mem *mem,
                                                       struct kefir_ast_translator_type_cache *cache,
                                                       kefir_id_t ir_type_id,
                                                       struct kefir_ir_type *ir_type,
                                                       struct kefir_ast_type_layout *type_layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator alignned type cache"));
    REQUIRE(ir_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(type_layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type layout"));
    REQUIRE(type_layout->properties.valid, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected provided type layout to have valid properties"));

    struct kefir_ast_translator_cached_type *cached_type =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_cached_type));
    REQUIRE(cached_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator cached type"));

    cached_type->klass = KEFIR_AST_TRANSLATOR_CACHED_OBJECT_TYPE;
    cached_type->object.ir_type_id = ir_type_id;
    cached_type->object.ir_type = ir_type;
    cached_type->object.type_layout = type_layout;
    cached_type->object.layout_owner = false;
    
    kefir_result_t res = kefir_ast_translator_type_cache_insert(mem, cache, cached_type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, cached_type);
        REQUIRE(res == KEFIR_ALREADY_EXISTS, res);
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_cache_insert_unowned_function(struct kefir_mem *mem,
                                                                   struct kefir_ast_translator_type_cache *cache,
                                                                   struct kefir_ast_translator_function_declaration *decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator alignned type cache"));
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator function declaration"));
    
    struct kefir_ast_translator_cached_type *cached_type =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_cached_type));
    REQUIRE(cached_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator cached type"));

    cached_type->klass = KEFIR_AST_TRANSLATOR_CACHED_FUNCTION_TYPE;
    cached_type->function.declaration = decl;
    cached_type->function.decl_owner = false;
    
    kefir_result_t res = kefir_ast_translator_type_cache_insert(mem, cache, cached_type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, cached_type);
        REQUIRE(res == KEFIR_ALREADY_EXISTS, res);
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_cache_generate_owned_object(struct kefir_mem *mem,
                                                                 const struct kefir_ast_type *type,
                                                                 kefir_size_t alignment,
                                                                 struct kefir_ast_translator_type_cache *cache,
                                                                 const struct kefir_ast_translator_environment *env,
                                                                 struct kefir_ir_module *module,
                                                                 const struct kefir_ast_translator_cached_type **cached_type_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type cache"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(cached_type_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator cached type pointer"));

    kefir_result_t res = kefir_ast_translator_type_cache_at(cache, type, alignment, cached_type_ptr);
    if (res == KEFIR_NOT_FOUND) {
        struct kefir_ast_translator_cached_type *cached_type =
            KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_cached_type));
        REQUIRE(cached_type != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator cached type"));
        cached_type->klass = KEFIR_AST_TRANSLATOR_CACHED_OBJECT_TYPE;

        cached_type->object.ir_type = kefir_ir_module_new_type(mem, module, 0, &cached_type->object.ir_type_id);
        struct kefir_irbuilder_type type_builder;
        res = kefir_irbuilder_type_init(mem, &type_builder, cached_type->object.ir_type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_IRBUILDER_TYPE_FREE(&type_builder);
            KEFIR_FREE(mem, cached_type);
            return res;
        });
        
        res = kefir_ast_translate_object_type(mem, type, alignment, env, &type_builder, &cached_type->object.type_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_IRBUILDER_TYPE_FREE(&type_builder);
            KEFIR_FREE(mem, cached_type);
            return res;
        });
        cached_type->object.layout_owner = true;
        
        res = KEFIR_IRBUILDER_TYPE_FREE(&type_builder);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, cached_type->object.type_layout);
            KEFIR_FREE(mem, cached_type);
            return res;
        });

        res = kefir_ast_translator_evaluate_type_layout(mem, env, cached_type->object.type_layout, cached_type->object.ir_type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, cached_type->object.type_layout);
            KEFIR_FREE(mem, cached_type);
            return res;
        });

        res = kefir_ast_translator_type_cache_insert(mem, cache, cached_type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, cached_type->object.type_layout);
            KEFIR_FREE(mem, cached_type);
            return res;
        });
        *cached_type_ptr = cached_type;
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_cache_generate_owned_function(struct kefir_mem *mem,
                                                                   const struct kefir_ast_type *type,
                                                                   struct kefir_ast_translator_type_cache *cache,
                                                                   const struct kefir_ast_translator_environment *env,
                                                                   const struct kefir_ast_type_traits *type_traits,
                                                                   struct kefir_ir_module *module,
                                                                   const struct kefir_ast_translator_cached_type **cached_type_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL && type->tag == KEFIR_AST_TYPE_FUNCTION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(cache != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type cache"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(cached_type_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator cached type pointer"));

    kefir_result_t res = kefir_ast_translator_type_cache_at(cache, type, 0, cached_type_ptr);
    if (res == KEFIR_NOT_FOUND) {
        struct kefir_ast_translator_cached_type *cached_type =
            KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_cached_type));
        REQUIRE(cached_type != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator cached type"));
        cached_type->klass = KEFIR_AST_TRANSLATOR_CACHED_FUNCTION_TYPE;

        res = kefir_ast_translator_function_declaration_init(mem, env, type_traits, module, cache, type, &cached_type->function.declaration);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, cached_type);
            return res;
        });
        cached_type->function.decl_owner = true;

        res = kefir_ast_translator_type_cache_insert(mem, cache, cached_type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_translator_function_declaration_free(mem, cached_type->function.declaration);
            KEFIR_FREE(mem, cached_type);
            return res;
        });
        *cached_type_ptr = cached_type;
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}