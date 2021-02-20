#ifndef KEFIR_AST_TYPE_H_
#define KEFIR_AST_TYPE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/list.h"
#include "kefir/core/hashtree.h"
#include "kefir/core/symbol_table.h"
#include "kefir/ast/type/base.h"
#include "kefir/ast/type/basic.h"
#include "kefir/ast/type/struct.h"
#include "kefir/ast/type/enum.h"
#include "kefir/ast/type/qualified.h"
#include "kefir/ast/type/array.h"
#include "kefir/ast/type/pointer.h"
#include "kefir/ast/type/function.h"

typedef struct kefir_ast_type_ops {
    kefir_bool_t (*same)(const struct kefir_ast_type *, const struct kefir_ast_type *);
    kefir_result_t (*free)(struct kefir_mem *, const struct kefir_ast_type *);
} kefir_ast_type_ops_t;

typedef struct kefir_ast_type {
    kefir_ast_type_tag_t tag;
    kefir_bool_t basic;
    struct kefir_ast_type_ops ops;
    union {
        struct kefir_ast_basic_type_properties basic_type;
        const struct kefir_ast_type *referenced_type;
        struct kefir_ast_enum_type enumeration_type;
        struct kefir_ast_struct_type structure_type;
        struct kefir_ast_array_type array_type;
        struct kefir_ast_function_type function_type;
        struct kefir_ast_qualified_type qualified_type;
    };
} kefir_ast_type_t;

typedef struct kefir_ast_type_traits {
    kefir_result_t (*integral_type_fits)(const struct kefir_ast_type_traits *,
                                    const struct kefir_ast_type *,
                                    const struct kefir_ast_type *,
                                    kefir_bool_t *);

    void *payload;
} kefir_ast_type_traits_t;

typedef struct kefir_ast_type_repository {
    struct kefir_symbol_table *symbols;
    struct kefir_list types;
} kefir_ast_type_repository_t;

const struct kefir_ast_type_traits *kefir_ast_default_type_traits();

kefir_result_t kefir_ast_type_repository_init(struct kefir_ast_type_repository *, struct kefir_symbol_table *);
kefir_result_t kefir_ast_type_repository_free(struct kefir_mem *, struct kefir_ast_type_repository *);

#define KEFIR_AST_TYPE_SAME(type1, type2) ((type1)->ops.same((type1), (type2)))
// TODO: Define type compatibility function according to the standard
#define KEFIR_AST_TYPE_COMPATIBLE(type1, type2) KEFIR_AST_TYPE_SAME((type1), (type2))
#define KEFIR_AST_TYPE_FREE(mem, type) ((type)->ops.free((mem), (type)))

#endif