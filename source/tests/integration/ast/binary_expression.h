#ifndef BINARY_EXPRESSION_H_
#define BINARY_EXPRESSION_H_

#define BINARY_NODE(_oper, _node1, _node2) \
    do { \
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem, (_oper), (_node1), (_node2))); \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node)); \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context)); \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node)); \
    } while (0)

#define FUNC(_id, _init) \
    do { \
        struct kefir_ir_type *func_params = kefir_ir_module_new_type(mem, &module, 0, NULL); \
        struct kefir_ir_type *func_returns = kefir_ir_module_new_type(mem, &module, 0, NULL); \
        struct kefir_ir_function_decl *func_decl = kefir_ir_module_new_named_function_declaration(mem, &module, \
            (_id), func_params, false, func_returns); \
        REQUIRE(func_decl != NULL, KEFIR_INTERNAL_ERROR); \
        struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, func_decl, \
            translator_local_scope.local_layout, 0); \
        REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body)); \
        _init \
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder)); \
    } while (0)

#endif