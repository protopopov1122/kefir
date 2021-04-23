#ifndef KEFIR_AST_TRANSLATOR_FUNCTION_DECLARATION_H_
#define KEFIR_AST_TRANSLATOR_FUNCTION_DECLARATION_H_

#include "kefir/ast/type.h"
#include "kefir/ir/module.h"
#include "kefir/ast/type_layout.h"
#include "kefir/core/list.h"
#include "kefir/core/hashtree.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ast-translator/base.h"

typedef struct kefir_ast_translator_function_declaration {
    const struct kefir_ast_type *function_type;
    struct kefir_ir_function_decl *ir_function_decl;
    kefir_id_t ir_argument_type_id;
    struct kefir_ir_type *ir_argument_type;
    kefir_id_t ir_return_type_id;
    struct kefir_ir_type *ir_return_type;

    struct kefir_list argument_layouts;
    struct kefir_hashtree named_argument_layouts;
    struct kefir_ast_type_layout *return_layout;
} kefir_ast_translator_function_declaration_t;

kefir_result_t kefir_ast_translator_function_declaration_init(struct kefir_mem *,
                                                          const struct kefir_ast_translator_environment *,
                                                          const struct kefir_ast_type_traits *,
                                                          struct kefir_ir_module *,
                                                          struct kefir_ast_translator_type_cache *,
                                                          const struct kefir_ast_type *,
                                                          struct kefir_ast_translator_function_declaration **);

kefir_result_t kefir_ast_translator_function_declaration_free(struct kefir_mem *,
                                                          struct kefir_ast_translator_function_declaration *);

kefir_result_t kefir_ast_translator_function_declaration_init_vararg(struct kefir_mem *,
                                                                 const struct kefir_ast_translator_environment *,
                                                                 const struct kefir_ast_type_traits *,
                                                                 struct kefir_ir_module *,
                                                                 struct kefir_ast_translator_type_cache *,
                                                                 const struct kefir_ast_type *,
                                                                 const struct kefir_list *,
                                                                 struct kefir_ast_translator_function_declaration **);

#endif