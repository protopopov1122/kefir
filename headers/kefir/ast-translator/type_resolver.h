#ifndef KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_H_
#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_H_

#include "kefir/ir/type.h"
#include "kefir/ast/type.h"
#include "kefir/ast/type_layout.h"
#include "kefir/ast-translator/function_declaration.h"

typedef enum kefir_ast_translator_resolved_type_class {
    KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
    KEFIR_AST_TRANSLATOR_RESOLVED_FUNCTION_TYPE
} kefir_ast_translator_resolved_type_class_t;

typedef struct kefir_ast_translator_resolved_type {
    kefir_ast_translator_resolved_type_class_t klass;
    kefir_bool_t owner;
    union {
        struct {
            kefir_id_t ir_type_id;
            struct kefir_ir_type *ir_type;
            struct kefir_ast_type_layout *layout;
        } object;

        struct {
            struct kefir_ast_translator_function_declaration *declaration;
        } function;
    };
} kefir_ast_translator_resolved_type_t;

typedef struct kefir_ast_translator_type_resolver {
    kefir_result_t (*resolve)(const struct kefir_ast_translator_type_resolver *, const struct kefir_ast_type *,
                              kefir_size_t, const struct kefir_ast_translator_resolved_type **);

    kefir_result_t (*register_object)(struct kefir_mem *, const struct kefir_ast_translator_type_resolver *, kefir_id_t,
                                      struct kefir_ir_type *, struct kefir_ast_type_layout *);

    kefir_result_t (*register_function)(struct kefir_mem *, const struct kefir_ast_translator_type_resolver *,
                                        struct kefir_ast_translator_function_declaration *);

    kefir_result_t (*build_object)(struct kefir_mem *, struct kefir_ast_translator_type_resolver *,
                                   const struct kefir_ast_translator_environment *, struct kefir_ir_module *,
                                   const struct kefir_ast_type *, kefir_size_t,
                                   const struct kefir_ast_translator_resolved_type **);

    kefir_result_t (*build_function)(struct kefir_mem *, struct kefir_ast_translator_type_resolver *,
                                     const struct kefir_ast_translator_environment *, struct kefir_ast_type_bundle *,
                                     const struct kefir_ast_type_traits *, struct kefir_ir_module *,
                                     const struct kefir_ast_type *, const struct kefir_ast_translator_resolved_type **);

    kefir_result_t (*free)(struct kefir_mem *, struct kefir_ast_translator_type_resolver *);

    void *payload;
} kefir_ast_translator_type_resolver_t;

#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_RESOLVE(_resolver, _type, _alignment, _resolved_type) \
    ((_resolver)->resolve((_resolver), (_type), (_alignment), (_resolved_type)))
#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_REGISTER_OBJECT(_mem, _resolver, _id, _ir_type, _layout) \
    ((_resolver)->register_object((_mem), (_resolver), (_id), (_ir_type), (_layout)))
#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_REGISTER_FUNCTION(_mem, _resolver, _declaration) \
    ((_resolver)->register_function((_mem), (_resolver), (_declaration)))
#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(_mem, _resolver, _env, _module, _type, _alignment, \
                                                        _resolved_type)                                    \
    ((_resolver)->build_object((_mem), (_resolver), (_env), (_module), (_type), (_alignment), (_resolved_type)))
#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_FUNCTION(_mem, _resolver, _env, _type_bundle, _type_traits, _module, \
                                                          _type, _resolved_type)                                      \
    ((_resolver)->build_function((_mem), (_resolver), (_env), (_type_bundle), (_type_traits), (_module), (_type),     \
                                 (_resolved_type)))
#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_FREE(_mem, _resolver) ((_resolver)->free((_mem), (_resolver)))

#endif
