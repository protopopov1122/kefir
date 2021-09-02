VALGRIND_OPTIONS=-q --trace-children=yes --track-origins=yes --expensive-definedness-checks=yes --leak-check=full --error-exitcode=127
CC_TEST_FLAGS=-no-pie

ifeq ($(PLATFORM),freebsd)
VALGRIND_OPTIONS+=--suppressions=$(ROOT)/suppressions/freebsd.valgrind
CC_TEST_FLAGS=
endif

include source/tests/unit/Makefile.mk
include source/tests/integration/Makefile.mk
include source/tests/system/Makefile.mk
include source/tests/end2end/Makefile.mk