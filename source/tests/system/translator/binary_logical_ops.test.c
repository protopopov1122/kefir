#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

typedef bool (*callback_t)(void *);

bool logical_and(callback_t, callback_t, void *);
bool logical_or(callback_t, callback_t, void *);

struct param {
    int fn_true;
    int fn_false;
};

bool fn_true(void *payload) {
    struct param *p = (struct param *) payload;
    p->fn_true++;
    return true;
}

bool fn_false(void *payload) {
    struct param *p = (struct param *) payload;
    p->fn_false++;
    return false;
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

#define ASSERT_AND(_fn1, _fn2, _res, _true, _false) \
    do { \
        struct param p = {0}; \
        ASSERT(logical_and((_fn1), (_fn2), &p) == (_res)); \
        ASSERT(p.fn_true == (_true)); \
        ASSERT(p.fn_false == (_false)); \
    } while (0)

    ASSERT_AND(fn_false, fn_false, false, 0, 1);
    ASSERT_AND(fn_false, fn_true, false, 0, 1);
    ASSERT_AND(fn_true, fn_false, false, 1, 1);
    ASSERT_AND(fn_true, fn_true, true, 2, 0);

#define ASSERT_OR(_fn1, _fn2, _res, _true, _false) \
    do { \
        struct param p = {0}; \
        ASSERT(logical_or((_fn1), (_fn2), &p) == (_res)); \
        ASSERT(p.fn_true == (_true)); \
        ASSERT(p.fn_false == (_false)); \
    } while (0)

    ASSERT_OR(fn_false, fn_false, false, 0, 2);
    ASSERT_OR(fn_false, fn_true, true, 1, 1);
    ASSERT_OR(fn_true, fn_false, true, 1, 0);
    ASSERT_OR(fn_true, fn_true, true, 1, 0);
    return EXIT_SUCCESS;
}