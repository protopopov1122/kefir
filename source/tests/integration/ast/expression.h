#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#define UNARY_NODE(_oper, _node)                                                                                      \
    do {                                                                                                              \
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(mem, (_oper), (_node))); \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));                                                       \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));                         \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));                                                                   \
    } while (0)

#define BINARY_NODE(_oper, _node1, _node2)                                                         \
    do {                                                                                           \
        struct kefir_ast_node_base *node =                                                         \
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem, (_oper), (_node1), (_node2))); \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));                                    \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));      \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));                                                \
    } while (0)

#define FUNC(_id, _init)                                                                                     \
    do {                                                                                                     \
        struct kefir_ir_type *func_params = kefir_ir_module_new_type(mem, &module, 0, NULL);                 \
        struct kefir_ir_type *func_returns = kefir_ir_module_new_type(mem, &module, 0, NULL);                \
        struct kefir_ir_function_decl *func_decl =                                                           \
            kefir_ir_module_new_function_declaration(mem, &module, (_id), func_params, false, func_returns); \
        REQUIRE(func_decl != NULL, KEFIR_INTERNAL_ERROR);                                                    \
        struct kefir_ir_function *func =                                                                     \
            kefir_ir_module_new_function(mem, &module, func_decl, translator_local_scope.local_layout, 0);   \
        REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body));                                  \
        _init REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));                                              \
    } while (0)

#define FUNC2(_id, _init)                                                                                    \
    do {                                                                                                     \
        struct kefir_ast_local_context local_context;                                                        \
        REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));                      \
        struct kefir_ast_context *context = &local_context.context;                                          \
                                                                                                             \
        struct kefir_ast_translator_local_scope_layout translator_local_scope;                               \
        REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope,      \
                                                                &translator_local_scope));                   \
        struct kefir_ast_translator_context local_translator_context;                                        \
        REQUIRE_OK(kefir_ast_translator_context_init(&local_translator_context, context, &env, &module));    \
        struct kefir_ir_type *func_params = kefir_ir_module_new_type(mem, &module, 0, NULL);                 \
        struct kefir_ir_type *func_returns = kefir_ir_module_new_type(mem, &module, 0, NULL);                \
        struct kefir_ir_function_decl *func_decl =                                                           \
            kefir_ir_module_new_function_declaration(mem, &module, (_id), func_params, false, func_returns); \
        REQUIRE(func_decl != NULL, KEFIR_INTERNAL_ERROR);                                                    \
        struct kefir_ir_function *func =                                                                     \
            kefir_ir_module_new_function(mem, &module, func_decl, translator_local_scope.local_layout, 0);   \
        REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body));                                  \
        _init REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));                                              \
        REQUIRE_OK(kefir_ast_translator_context_free(mem, &local_translator_context));                       \
        REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));              \
        REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));                                       \
    } while (0)

#endif
