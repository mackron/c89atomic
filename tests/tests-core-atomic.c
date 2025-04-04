#include <limits.h>
#include <stdint.h>
#include <c89atomic.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <thread.h>

/* Test atomic_flag back and forth */
static void test_atomic_flag(void) {
    atomic_flag flag = 0;

    CHK_EXPECTED(0, atomic_flag_test_and_set(&flag));
    CHK_EXPECTED(1, atomic_flag_test_and_set(&flag));
    atomic_flag_clear(&flag);
    CHK_EXPECTED(0, atomic_flag_test_and_set(&flag));
}

/* Test atomic_fetch_add */
static void test_atomic_inc_positive(void) {
    atomic_size_t res;
    atomic_init(&res, 0);
    int i;

    CHK_EXPECTED(0, atomic_fetch_add(&res, 1));
    CHK_EXPECTED(1, atomic_load(&res));
    CHK_EXPECTED(1, atomic_fetch_add(&res, 1));
    CHK_EXPECTED(2, atomic_load(&res));
    atomic_store(&res, 0);
    for (i = 0; i < 512; ++i) {
        CHK_EXPECTED(i, atomic_fetch_add(&res, 1));
        CHK_EXPECTED(i + 1, atomic_load(&res));
    }
}

static void test_atomic_inc_negative(void) {
    atomic_size_t res;
    atomic_init(&res, -99);
    int i;

    for (i = -99; i < 123; ++i) {
        CHK_EXPECTED(i, atomic_fetch_add(&res, 1));
        CHK_EXPECTED(i + 1, atomic_load(&res));
    }
}

static void test_atomic_inc_rollover(void) {
    atomic_size_t res;
    atomic_init(&res, INT_MAX - 30);
    int i;

    for (i = 0; i < 30; ++i) {
        CHK_EXPECTED(INT_MAX - 30 + i, atomic_fetch_add(&res, 1));
        CHK_EXPECTED(INT_MAX - 30 + i + 1, atomic_load(&res));
    }
    CHK_EXPECTED(INT_MAX, atomic_fetch_add(&res, 1));
    CHK_EXPECTED(INT_MIN, atomic_load(&res));
    CHK_EXPECTED(INT_MIN, atomic_fetch_add(&res, 1));
    CHK_EXPECTED(INT_MIN + 1, atomic_load(&res));
    CHK_EXPECTED(INT_MIN + 1, atomic_fetch_add(&res, 1));
    CHK_EXPECTED(INT_MIN + 2, atomic_load(&res));
}

/* Test atomic_fetch_sub */
static void test_atomic_dec_negative(void) {
    atomic_size_t res;
    atomic_init(&res, 0);
    int i;

    CHK_EXPECTED(0, atomic_fetch_sub(&res, 1));
    CHK_EXPECTED(-1, atomic_load(&res));
    CHK_EXPECTED(-1, atomic_fetch_sub(&res, 1));
    CHK_EXPECTED(-2, atomic_load(&res));
    atomic_store(&res, 0);
    for (i = 0; i < 512; ++i) {
        CHK_EXPECTED(-i, atomic_fetch_sub(&res, 1));
        CHK_EXPECTED(-i - 1, atomic_load(&res));
    }
}

static void test_atomic_dec_positive(void) {
    atomic_size_t res;
    atomic_init(&res, 99);
    int i;

    for (i = 99; i > -123; --i) {
        CHK_EXPECTED(i, atomic_fetch_sub(&res, 1));
        CHK_EXPECTED(i - 1, atomic_load(&res));
    }
}

static void test_atomic_dec_rollover(void) {
    atomic_size_t res;
    atomic_init(&res, INT_MIN + 30);
    int i;

    for (i = 0; i < 30; ++i) {
        CHK_EXPECTED(INT_MIN + 30 - i, atomic_fetch_sub(&res, 1));
        CHK_EXPECTED(INT_MIN + 30 - i - 1, atomic_load(&res));
    }
    CHK_EXPECTED(INT_MIN, atomic_fetch_sub(&res, 1));
    CHK_EXPECTED(INT_MAX, atomic_load(&res));
    CHK_EXPECTED(INT_MAX, atomic_fetch_sub(&res, 1));
    CHK_EXPECTED(INT_MAX - 1, atomic_load(&res));
    CHK_EXPECTED(INT_MAX - 1, atomic_fetch_sub(&res, 1));
    CHK_EXPECTED(INT_MAX - 2, atomic_load(&res));
}

/* Test atomic_cas with a correct old value */
static void test_atomic_cas_same(void) {
    atomic_size_t res;
    atomic_init(&res, 0);
    size_t expected;

    expected = 0;
    CHK_EXPECTED(1, atomic_compare_exchange_weak(&res, &expected, 12345));
    CHK_EXPECTED(12345, atomic_load(&res));
    CHK_EXPECTED(0, expected);
    expected = 12345;
    CHK_EXPECTED(1, atomic_compare_exchange_weak(&res, &expected, -9876));
    CHK_EXPECTED(-9876, atomic_load(&res));
    CHK_EXPECTED(12345, expected);
    expected = -9876;
    CHK_EXPECTED(1, atomic_compare_exchange_weak(&res, &expected, -9876));
    CHK_EXPECTED(-9876, atomic_load(&res));
    CHK_EXPECTED(-9876, expected);
    expected = -9876;
    CHK_EXPECTED(1, atomic_compare_exchange_weak(&res, &expected, 0));
    CHK_EXPECTED(0, atomic_load(&res));
}

/* Test atomic_cas with a non-matching old value */
static void test_atomic_cas_diff(void) {
    atomic_size_t res;
    atomic_init(&res, 32767);
    size_t expected;

    expected = 22222;
    CHK_EXPECTED(0, atomic_compare_exchange_weak(&res, &expected, 12345));
    CHK_EXPECTED(32767, expected);
    CHK_EXPECTED(32767, atomic_load(&res));

    atomic_store(&res, -12345);
    expected = 12345;
    CHK_EXPECTED(0, atomic_compare_exchange_weak(&res, &expected, 12345));
    CHK_EXPECTED(-12345, expected);
    CHK_EXPECTED(-12345, atomic_load(&res));

    expected = 12345;
    CHK_EXPECTED(0, atomic_compare_exchange_weak(&res, &expected, 12345));
    CHK_EXPECTED(-12345, expected);
    CHK_EXPECTED(-12345, atomic_load(&res));

    expected = 12345;
    CHK_EXPECTED(0, atomic_compare_exchange_weak(&res, &expected, -12345));
    CHK_EXPECTED(-12345, expected);
    CHK_EXPECTED(-12345, atomic_load(&res));
}

/* Test atomic_load, atomic_store */
static void test_atomic_value(void) {
    atomic_size_t res;
    atomic_init(&res, 12345);

    CHK_EXPECTED(12345, atomic_load(&res));
    atomic_store(&res, 24332);
    CHK_EXPECTED(24332, atomic_load(&res));
}

/* `atomic_init` is implicitly tested by the other tests */
int main(void) {
    puts("Test atomic_flag back and forth");
    test_atomic_flag();

    puts("Test atomic_fetch_add");
    test_atomic_inc_positive();

    puts("Test atomic_fetch_add reversed");
    test_atomic_inc_negative();

    puts("Test atomic_fetch_add rolled over");
    test_atomic_inc_rollover();

    puts("Test atomic_fetch_sub");
    test_atomic_dec_negative();

    puts("Test atomic_fetch_sub forward");
    test_atomic_dec_positive();

    puts("Test atomic_fetch_sub rolled over");
    test_atomic_dec_rollover();

    puts("Test atomic_cas with a correct old value");
    test_atomic_cas_same();

    puts("Test atomic_cas with a non-matching old value");
    test_atomic_cas_diff();

    puts("Test atomic_load, atomic_store");
    test_atomic_value();

    puts("tests finished");

    return 0;
}
