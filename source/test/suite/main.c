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

TEST_SUITE(mainSuite,
    &amd64_sysv_abi_data_test1,
    &amd64_sysv_abi_data_test2,
    &amd64_sysv_abi_data_test3,
    &amd64_sysv_abi_data_test4,
    &amd64_sysv_abi_data_test5,
    &amd64_sysv_abi_data_test6,
    &amd64_sysv_abi_data_test7,
    &amd64_sysv_abi_data_test8,
    &amd64_sysv_abi_data_test9
);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    return kft_run_test_suite(mainSuite, mainSuiteLength, NULL);
}