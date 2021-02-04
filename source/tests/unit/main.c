#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"
#include <assert.h>

DECLARE_CASE(amd64_sysv_abi_data_test1);
DECLARE_CASE(amd64_sysv_abi_data_test2);
DECLARE_CASE(amd64_sysv_abi_data_test3);
DECLARE_CASE(amd64_sysv_abi_data_test4);
DECLARE_CASE(amd64_sysv_abi_data_test5);
DECLARE_CASE(amd64_sysv_abi_data_test6);
DECLARE_CASE(amd64_sysv_abi_data_test7);
DECLARE_CASE(amd64_sysv_abi_data_test8);
DECLARE_CASE(amd64_sysv_abi_data_test9);
DECLARE_CASE(amd64_sysv_abi_classification_test1);
DECLARE_CASE(amd64_sysv_abi_classification_test2);
DECLARE_CASE(amd64_sysv_abi_classification_test3);
DECLARE_CASE(amd64_sysv_abi_classification_test4);
DECLARE_CASE(amd64_sysv_abi_classification_test5);
DECLARE_CASE(amd64_sysv_abi_classification_test6);
DECLARE_CASE(amd64_sysv_abi_classification_test7);
DECLARE_CASE(amd64_sysv_abi_classification_test8);
DECLARE_CASE(amd64_sysv_abi_classification_test9);
DECLARE_CASE(amd64_sysv_abi_classification_test10);
DECLARE_CASE(amd64_sysv_abi_classification_test11);
DECLARE_CASE(amd64_sysv_abi_classification_test12);
DECLARE_CASE(amd64_sysv_abi_classification_test13);
DECLARE_CASE(amd64_sysv_abi_classification_test14);
DECLARE_CASE(amd64_sysv_abi_classification_test15);
DECLARE_CASE(amd64_sysv_abi_allocation_test1);
DECLARE_CASE(amd64_sysv_abi_allocation_test2);
DECLARE_CASE(amd64_sysv_abi_allocation_test3);
DECLARE_CASE(amd64_sysv_abi_allocation_test4);
DECLARE_CASE(amd64_sysv_abi_allocation_test5);
DECLARE_CASE(amd64_sysv_abi_allocation_test6);

DECLARE_CASE(ast_type_analysis_integer_promotion1);

TEST_SUITE(mainSuite,
    &amd64_sysv_abi_data_test1,
    &amd64_sysv_abi_data_test2,
    &amd64_sysv_abi_data_test3,
    &amd64_sysv_abi_data_test4,
    &amd64_sysv_abi_data_test5,
    &amd64_sysv_abi_data_test6,
    &amd64_sysv_abi_data_test7,
    &amd64_sysv_abi_data_test8,
    &amd64_sysv_abi_data_test9,
    &amd64_sysv_abi_classification_test1,
    &amd64_sysv_abi_classification_test2,
    &amd64_sysv_abi_classification_test3,
    &amd64_sysv_abi_classification_test4,
    &amd64_sysv_abi_classification_test5,
    &amd64_sysv_abi_classification_test6,
    &amd64_sysv_abi_classification_test7,
    &amd64_sysv_abi_classification_test8,
    &amd64_sysv_abi_classification_test9,
    &amd64_sysv_abi_classification_test10,
    &amd64_sysv_abi_classification_test11,
    &amd64_sysv_abi_classification_test12,
    &amd64_sysv_abi_classification_test13,
    &amd64_sysv_abi_classification_test14,
    &amd64_sysv_abi_classification_test15,
    &amd64_sysv_abi_allocation_test1,
    &amd64_sysv_abi_allocation_test2,
    &amd64_sysv_abi_allocation_test3,
    &amd64_sysv_abi_allocation_test4,
    &amd64_sysv_abi_allocation_test5,
    &amd64_sysv_abi_allocation_test6,
    &ast_type_analysis_integer_promotion1
);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    return kft_run_test_suite(mainSuite, mainSuiteLength, NULL);
}