VALGRIND_OPTIONS=-q --trace-children=yes --track-origins=yes --expensive-definedness-checks=yes --leak-check=full --error-exitcode=127
VALGRIND_TEST_SUPPRESSIONS=

ifneq ($(VALGRIND_TEST_SUPPRESSIONS),)
VALGRIND_OPTIONS+=--suppressions=$(VALGRIND_TEST_SUPPRESSIONS)
endif

include source/tests/unit/Makefile.mk
include source/tests/integration/Makefile.mk
include source/tests/system/Makefile.mk
include source/tests/end2end/Makefile.mk