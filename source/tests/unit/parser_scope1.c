#include "kefir/test/unit_test.h"
#include "kefir/parser/scope.h"

DEFINE_CASE(parser_block_scope1, "Parser - block scope #1") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_parser_block_scope block_scope;
    ASSERT_OK(kefir_parser_block_scope_init(&block_scope, &symbols));

    kefir_bool_t result;
    ASSERT(kefir_parser_block_scope_is_typedef(&block_scope, "T1", &result) == KEFIR_NOT_FOUND);
    ASSERT(kefir_parser_block_scope_is_typedef(&block_scope, "T2", &result) == KEFIR_NOT_FOUND);
    ASSERT(kefir_parser_block_scope_is_typedef(&block_scope, "T3", &result) == KEFIR_NOT_FOUND);

    ASSERT_OK(kefir_parser_block_scope_declare_typedef(&kft_mem, &block_scope, "T1"));
    ASSERT_OK(kefir_parser_block_scope_declare_variable(&kft_mem, &block_scope, "T2"));
    ASSERT_OK(kefir_parser_block_scope_declare_typedef(&kft_mem, &block_scope, "T3"));

    ASSERT_OK(kefir_parser_block_scope_is_typedef(&block_scope, "T1", &result));
    ASSERT(result);
    ASSERT_OK(kefir_parser_block_scope_is_typedef(&block_scope, "T2", &result));
    ASSERT(!result);
    ASSERT_OK(kefir_parser_block_scope_is_typedef(&block_scope, "T3", &result));
    ASSERT(result);

    ASSERT_OK(kefir_parser_block_scope_declare_typedef(&kft_mem, &block_scope, "T1"));
    ASSERT_OK(kefir_parser_block_scope_declare_variable(&kft_mem, &block_scope, "T2"));
    ASSERT_OK(kefir_parser_block_scope_declare_typedef(&kft_mem, &block_scope, "T3"));

    ASSERT(kefir_parser_block_scope_declare_variable(&kft_mem, &block_scope, "T1") == KEFIR_ALREADY_EXISTS);
    ASSERT(kefir_parser_block_scope_declare_typedef(&kft_mem, &block_scope, "T2") == KEFIR_ALREADY_EXISTS);
    ASSERT(kefir_parser_block_scope_declare_variable(&kft_mem, &block_scope, "T3") == KEFIR_ALREADY_EXISTS);

    ASSERT_OK(kefir_parser_block_scope_is_typedef(&block_scope, "T1", &result));
    ASSERT(result);
    ASSERT_OK(kefir_parser_block_scope_is_typedef(&block_scope, "T2", &result));
    ASSERT(!result);
    ASSERT_OK(kefir_parser_block_scope_is_typedef(&block_scope, "T3", &result));
    ASSERT(result);

    ASSERT_OK(kefir_parser_block_scope_declare_variable(&kft_mem, &block_scope, "T4"));
    ASSERT_OK(kefir_parser_block_scope_is_typedef(&block_scope, "T4", &result));
    ASSERT(!result);
    ASSERT_OK(kefir_parser_block_scope_declare_variable(&kft_mem, &block_scope, "T4"));
    ASSERT(kefir_parser_block_scope_declare_typedef(&kft_mem, &block_scope, "T4") == KEFIR_ALREADY_EXISTS);

    ASSERT_OK(kefir_parser_block_scope_free(&kft_mem, &block_scope));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

#define ASSERT_TYPEDEF(_scope, _id, _result)                                \
    do {                                                                    \
        kefir_bool_t result;                                                \
        ASSERT_OK(kefir_parser_scope_is_typedef((_scope), (_id), &result)); \
        ASSERT(result == (_result));                                        \
    } while (0)

DEFINE_CASE(parser_scope1, "Parser - scope #1") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_parser_scope scope;
    ASSERT_OK(kefir_parser_scope_init(&kft_mem, &scope, &symbols));

    kefir_bool_t result;
    ASSERT(kefir_parser_scope_is_typedef(&scope, "T1", &result) == KEFIR_NOT_FOUND);
    ASSERT(kefir_parser_scope_is_typedef(&scope, "T2", &result) == KEFIR_NOT_FOUND);
    ASSERT(kefir_parser_scope_is_typedef(&scope, "T3", &result) == KEFIR_NOT_FOUND);

    ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T1"));
    ASSERT_OK(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T2"));
    ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T3"));

    ASSERT_TYPEDEF(&scope, "T1", true);
    ASSERT_TYPEDEF(&scope, "T2", false);
    ASSERT_TYPEDEF(&scope, "T3", true);

    ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T1"));
    ASSERT_OK(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T2"));
    ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T3"));

    ASSERT(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T1") == KEFIR_ALREADY_EXISTS);
    ASSERT(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T2") == KEFIR_ALREADY_EXISTS);
    ASSERT(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T3") == KEFIR_ALREADY_EXISTS);

    do {
        ASSERT_OK(kefir_parser_scope_push_block(&kft_mem, &scope));

        ASSERT_TYPEDEF(&scope, "T1", true);
        ASSERT_TYPEDEF(&scope, "T2", false);
        ASSERT_TYPEDEF(&scope, "T3", true);

        ASSERT_OK(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T1"));
        ASSERT_TYPEDEF(&scope, "T1", false);
        ASSERT_TYPEDEF(&scope, "T2", false);
        ASSERT_TYPEDEF(&scope, "T3", true);

        ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T2"));
        ASSERT_TYPEDEF(&scope, "T1", false);
        ASSERT_TYPEDEF(&scope, "T2", true);
        ASSERT_TYPEDEF(&scope, "T3", true);

        ASSERT(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T1") == KEFIR_ALREADY_EXISTS);
        ASSERT(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T2") == KEFIR_ALREADY_EXISTS);

        ASSERT_OK(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T3"));
        ASSERT_TYPEDEF(&scope, "T1", false);
        ASSERT_TYPEDEF(&scope, "T2", true);
        ASSERT_TYPEDEF(&scope, "T3", false);

        ASSERT_OK(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T4"));
        ASSERT_TYPEDEF(&scope, "T4", false);

        do {
            ASSERT_OK(kefir_parser_scope_push_block(&kft_mem, &scope));

            ASSERT_TYPEDEF(&scope, "T1", false);
            ASSERT_TYPEDEF(&scope, "T2", true);
            ASSERT_TYPEDEF(&scope, "T3", false);
            ASSERT_TYPEDEF(&scope, "T4", false);

            ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T4"));
            ASSERT_TYPEDEF(&scope, "T1", false);
            ASSERT_TYPEDEF(&scope, "T2", true);
            ASSERT_TYPEDEF(&scope, "T3", false);
            ASSERT_TYPEDEF(&scope, "T4", true);

            ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T5"));
            ASSERT_TYPEDEF(&scope, "T1", false);
            ASSERT_TYPEDEF(&scope, "T2", true);
            ASSERT_TYPEDEF(&scope, "T3", false);
            ASSERT_TYPEDEF(&scope, "T4", true);
            ASSERT_TYPEDEF(&scope, "T5", true);

            ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T1"));
            ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T2"));
            ASSERT_TYPEDEF(&scope, "T1", true);
            ASSERT_TYPEDEF(&scope, "T2", true);
            ASSERT_TYPEDEF(&scope, "T3", false);
            ASSERT_TYPEDEF(&scope, "T4", true);
            ASSERT_TYPEDEF(&scope, "T5", true);

            ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T1"));
            ASSERT(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T1") == KEFIR_ALREADY_EXISTS);
            ASSERT_TYPEDEF(&scope, "T1", true);
            ASSERT_TYPEDEF(&scope, "T2", true);
            ASSERT_TYPEDEF(&scope, "T3", false);
            ASSERT_TYPEDEF(&scope, "T4", true);
            ASSERT_TYPEDEF(&scope, "T5", true);

            ASSERT_OK(kefir_parser_scope_pop_block(&kft_mem, &scope));
        } while (0);

        ASSERT_TYPEDEF(&scope, "T1", false);
        ASSERT_TYPEDEF(&scope, "T2", true);
        ASSERT_TYPEDEF(&scope, "T3", false);
        ASSERT_TYPEDEF(&scope, "T4", false);
        ASSERT(kefir_parser_scope_is_typedef(&scope, "T5", &result) == KEFIR_NOT_FOUND);

        ASSERT_OK(kefir_parser_scope_pop_block(&kft_mem, &scope));
    } while (0);

    do {
        ASSERT_OK(kefir_parser_scope_push_block(&kft_mem, &scope));

        ASSERT_OK(kefir_parser_scope_declare_typedef(&kft_mem, &scope, "T4"));
        ASSERT_OK(kefir_parser_scope_declare_variable(&kft_mem, &scope, "T5"));
        ASSERT_TYPEDEF(&scope, "T1", true);
        ASSERT_TYPEDEF(&scope, "T2", false);
        ASSERT_TYPEDEF(&scope, "T3", true);
        ASSERT_TYPEDEF(&scope, "T4", true);
        ASSERT_TYPEDEF(&scope, "T5", false);

        ASSERT_OK(kefir_parser_scope_pop_block(&kft_mem, &scope));
    } while (0);

    ASSERT_TYPEDEF(&scope, "T1", true);
    ASSERT_TYPEDEF(&scope, "T2", false);
    ASSERT_TYPEDEF(&scope, "T3", true);
    ASSERT(kefir_parser_scope_is_typedef(&scope, "T4", &result) == KEFIR_NOT_FOUND);
    ASSERT(kefir_parser_scope_is_typedef(&scope, "T5", &result) == KEFIR_NOT_FOUND);

    ASSERT_OK(kefir_parser_scope_free(&kft_mem, &scope));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
