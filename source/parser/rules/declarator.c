#include "kefir/parser/rule_helpers.h"

static kefir_result_t scan_type_qualifier_list(struct kefir_mem *mem, struct kefir_parser *parser,
                                               struct kefir_ast_type_qualifier_list *type_qualifiers) {
    while (true) {
        if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_CONST)) {
            REQUIRE_OK(kefir_ast_type_qualifier_list_append(mem, type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_CONST));
        } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_VOLATILE)) {
            REQUIRE_OK(kefir_ast_type_qualifier_list_append(mem, type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_VOLATILE));
        } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_RESTRICT)) {
            REQUIRE_OK(kefir_ast_type_qualifier_list_append(mem, type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_RESTRICT));
        } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ATOMIC)) {
            REQUIRE_OK(kefir_ast_type_qualifier_list_append(mem, type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_ATOMIC));
        } else {
            break;
        }

        REQUIRE_OK(PARSER_SHIFT(parser));
    }
    return KEFIR_OK;
}

static kefir_result_t scan_pointer(struct kefir_mem *mem, struct kefir_parser *parser,
                                   struct kefir_ast_declarator **declarator_ptr) {
    REQUIRE_OK(PARSER_SHIFT(parser));
    struct kefir_ast_type_qualifier_list type_qualifiers;
    REQUIRE_OK(kefir_ast_type_qualifier_list_init(&type_qualifiers));

    kefir_result_t res = scan_type_qualifier_list(mem, parser, &type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
        return res;
    });

    struct kefir_ast_declarator *subdeclarator = NULL;
    res = kefir_parser_scan_declarator(mem, parser, &subdeclarator);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
        return res;
    });

    struct kefir_ast_declarator *declarator = kefir_ast_declarator_pointer(mem, subdeclarator);
    REQUIRE_ELSE(declarator != NULL, {
        kefir_ast_declarator_free(mem, subdeclarator);
        kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST pointer declarator");
    });

    res = kefir_ast_type_qualifier_list_clone(mem, &declarator->pointer.type_qualifiers, &type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, declarator);
        kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
        return res;
    });

    res = kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, declarator);
        return res;
    });

    *declarator_ptr = declarator;
    return KEFIR_OK;
}

static kefir_result_t scan_direct_declarator_base(struct kefir_mem *mem, struct kefir_parser *parser,
                                                  struct kefir_ast_declarator **declarator_ptr) {
    kefir_result_t res = KEFIR_OK;
    struct kefir_ast_declarator *base_declarator = NULL;

    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
        REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
        REQUIRE_CHAIN(&res, kefir_parser_scan_declarator(mem, parser, &base_declarator));
        REQUIRE_CHAIN_SET(&res, PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
                          KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected right parenthese"));
        REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
    } else {
        REQUIRE_CHAIN_SET(&res, PARSER_TOKEN_IS_IDENTIFIER(parser, 0),
                          KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected identifier"));
        base_declarator = kefir_ast_declarator_identifier(mem, parser->symbols,
                                                          kefir_parser_token_cursor_at(parser->cursor, 0)->identifier);
        REQUIRE_CHAIN_SET(&res, base_declarator != NULL,
                          KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST identifier declarator"));
        REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        if (base_declarator != NULL) {
            kefir_ast_declarator_free(mem, base_declarator);
        }
        return res;
    });

    *declarator_ptr = base_declarator;
    return KEFIR_OK;
}

static kefir_result_t scan_direct(struct kefir_mem *mem, struct kefir_parser *parser,
                                  struct kefir_ast_declarator **declarator_ptr) {
    struct kefir_ast_declarator *base_declarator = NULL;
    REQUIRE_OK(scan_direct_declarator_base(mem, parser, &base_declarator));

    // TODO Implement array and function declarators
    *declarator_ptr = base_declarator;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scan_declarator(struct kefir_mem *mem, struct kefir_parser *parser,
                                            struct kefir_ast_declarator **declarator_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(declarator_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to declarator"));

    kefir_size_t checkpoint;
    kefir_result_t res = KEFIR_OK;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));
    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_STAR)) {
        res = scan_pointer(mem, parser, declarator_ptr);
    } else {
        res = scan_direct(mem, parser, declarator_ptr);
    }

    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));
    } else {
        REQUIRE_OK(res);
    }

    return KEFIR_OK;
}
