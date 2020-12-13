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
DECLARE_CASE(amd64_sysv_abi_data_test10);
DECLARE_CASE(amd64_sysv_abi_data_test11);
DECLARE_CASE(amd64_sysv_abi_data_test12);
DECLARE_CASE(amd64_sysv_abi_data_test13);
DECLARE_CASE(amd64_sysv_abi_data_test14);
DECLARE_CASE(amd64_sysv_abi_data_test15);
DECLARE_CASE(amd64_sysv_abi_data_test16);
DECLARE_CASE(amd64_sysv_abi_data_test17);
DECLARE_CASE(amd64_sysv_abi_data_test18);
DECLARE_CASE(amd64_sysv_abi_data_test19);
DECLARE_CASE(amd64_sysv_abi_data_test20);
DECLARE_CASE(amd64_sysv_abi_data_test21);
DECLARE_CASE(amd64_sysv_abi_data_test22);

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
    &amd64_sysv_abi_data_test10,
    &amd64_sysv_abi_data_test11,
    &amd64_sysv_abi_data_test12,
    &amd64_sysv_abi_data_test13,
    &amd64_sysv_abi_data_test14,
    &amd64_sysv_abi_data_test15,
    &amd64_sysv_abi_data_test16,
    &amd64_sysv_abi_data_test17,
    &amd64_sysv_abi_data_test18,
    &amd64_sysv_abi_data_test19,
    &amd64_sysv_abi_data_test20,
    &amd64_sysv_abi_data_test21,
    &amd64_sysv_abi_data_test22
);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    return kft_run_test_suite(mainSuite, mainSuiteLength, NULL);
}