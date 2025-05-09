/*
Tests basic logic of all atomic functions. Does not test atomicity.
*/
#include <stdio.h>
#include <string.h>

//#define C89ATOMIC_MODERN_GCC
//#define C89ATOMIC_LEGACY_GCC
//#define C89ATOMIC_LEGACY_GCC_ASM
//#define C89ATOMIC_MODERN_MSVC
//#define C89ATOMIC_LEGACY_MSVC
//#define C89ATOMIC_LEGACY_MSVC_ASM
#include "../c89atomic.c"

#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wlong-long"
    #if defined(__clang__)
        #pragma GCC diagnostic ignored "-Wc++11-long-long"
    #endif
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
    #define C89ATOMIC_ULL(x) (c89atomic_uint64)(x##i64)
#else
    #define C89ATOMIC_ULL(x) (c89atomic_uint64)(x##ULL)
#endif

#if defined(_WIN32)
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

static void enable_colored_output(void)
{
    HANDLE hOut;
    DWORD mode;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return;
    }

    if (!GetConsoleMode(hOut, &mode)) {
        return;
    }

    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#else
static void enable_colored_output(void)
{
    /* Do nothing. */
}
#endif


#define PRINT_WIDTH 40

/* Need to use a non-0 value for the old value to test byte ordering stuff properly. */
#define OLD_VAL 42

static int g_ErrorCount = 0;

const char* c89atomic_memory_order_to_string(c89atomic_memory_order order)
{
    switch (order)
    {
        case c89atomic_memory_order_relaxed: return "c89atomic_memory_order_relaxed";
        case c89atomic_memory_order_consume: return "c89atomic_memory_order_consume";
        case c89atomic_memory_order_acquire: return "c89atomic_memory_order_acquire";
        case c89atomic_memory_order_release: return "c89atomic_memory_order_release";
        case c89atomic_memory_order_acq_rel: return "c89atomic_memory_order_acq_rel";
        case c89atomic_memory_order_seq_cst: return "c89atomic_memory_order_seq_cst";
    }

    return "[unknown memory order]";
}


void c89atomic_test_passed(void)
{
    printf("\033[32mPASSED\033[0m\n");
}

void c89atomic_test_failed(void)
{
    printf("\033[31mFAILED\033[0m\n");
    g_ErrorCount += 1;
}


#define c89atomic_test__basic__flag_test_and_set_explicit(order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_flag a = 0; \
        c89atomic_flag b = 0; \
        c89atomic_flag r = c89atomic_flag_test_and_set_explicit(&b, order); \
        if (a == r && b == 1) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

void c89atomic_test__basic__flag_test_and_set(void)
{
    printf("c89atomic_flag_test_and_set():\n");

    c89atomic_test__basic__flag_test_and_set_explicit(c89atomic_memory_order_relaxed);
    /*c89atomic_test__basic__flag_test_and_set_explicit(c89atomic_memory_order_consume);*/
    c89atomic_test__basic__flag_test_and_set_explicit(c89atomic_memory_order_acquire);
    c89atomic_test__basic__flag_test_and_set_explicit(c89atomic_memory_order_release);
    c89atomic_test__basic__flag_test_and_set_explicit(c89atomic_memory_order_acq_rel);
    c89atomic_test__basic__flag_test_and_set_explicit(c89atomic_memory_order_seq_cst);

    printf("\n");
}

#define c89atomic_test__basic__flag_clear_explicit(order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_flag a = 1; \
        c89atomic_flag_clear_explicit(&a, order); \
        if (a == 0) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

void c89atomic_test__basic__flag_clear(void)
{
    printf("c89atomic_flag_clear():\n");

    c89atomic_test__basic__flag_clear_explicit(c89atomic_memory_order_relaxed);
    c89atomic_test__basic__flag_clear_explicit(c89atomic_memory_order_release);
    c89atomic_test__basic__flag_clear_explicit(c89atomic_memory_order_seq_cst);

    printf("\n");
}

#define c89atomic_test__basic__flag_load_explicit(order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_flag a = 42; \
        c89atomic_flag b = c89atomic_flag_load_explicit(&a, order); \
        if (a == b) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

void c89atomic_test__basic__flag_load(void)
{
    printf("c89atomic_flag_load():\n");

    c89atomic_test__basic__flag_load_explicit(c89atomic_memory_order_relaxed);
    c89atomic_test__basic__flag_load_explicit(c89atomic_memory_order_consume);
    c89atomic_test__basic__flag_load_explicit(c89atomic_memory_order_acquire);
    /*c89atomic_test__basic__flag_load_explicit(c89atomic_memory_order_release);*/
    /*c89atomic_test__basic__flag_load_explicit(c89atomic_memory_order_acq_rel);*/
    c89atomic_test__basic__flag_load_explicit(c89atomic_memory_order_seq_cst);

    printf("\n");
}


#define c89atomic_test__basic_load_explicit(sizeInBits, src, order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_uint##sizeInBits a = src; \
        c89atomic_uint##sizeInBits b = c89atomic_load_explicit_##sizeInBits(&a, order); \
        if (a == b) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_load_n(sizeInBits, src) \
{ \
    printf("c89atomic_load_%d():\n", sizeInBits); \
    c89atomic_test__basic_load_explicit(sizeInBits, src, c89atomic_memory_order_relaxed); \
    c89atomic_test__basic_load_explicit(sizeInBits, src, c89atomic_memory_order_consume); \
    c89atomic_test__basic_load_explicit(sizeInBits, src, c89atomic_memory_order_acquire); \
    /*c89atomic_test__basic_load_explicit(sizeInBits, src, c89atomic_memory_order_release);*/ \
    /*c89atomic_test__basic_load_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel);*/ \
    c89atomic_test__basic_load_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__load(void)
{
    c89atomic_test__basic_load_n(8,  123);
    c89atomic_test__basic_load_n(16, 1234);
    c89atomic_test__basic_load_n(32, 123456);
    c89atomic_test__basic_load_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_store_explicit(sizeInBits, src, order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = src; \
        c89atomic_store_explicit_##sizeInBits(&a, b, order); \
        if (b == a) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_store_n(sizeInBits, src) \
{ \
    printf("c89atomic_store_%d():\n", sizeInBits); \
    c89atomic_test__basic_store_explicit(sizeInBits, src, c89atomic_memory_order_relaxed); \
    /*c89atomic_test__basic_store_explicit(sizeInBits, src, c89atomic_memory_order_consume);*/ \
    /*c89atomic_test__basic_store_explicit(sizeInBits, src, c89atomic_memory_order_acquire);*/ \
    c89atomic_test__basic_store_explicit(sizeInBits, src, c89atomic_memory_order_release); \
    /*c89atomic_test__basic_store_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel);*/ \
    c89atomic_test__basic_store_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__store(void)
{
    c89atomic_test__basic_store_n(8,  123);
    c89atomic_test__basic_store_n(16, 1234);
    c89atomic_test__basic_store_n(32, 123456);
    c89atomic_test__basic_store_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_exchange_explicit(sizeInBits, src, order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = OLD_VAL; \
        c89atomic_uint##sizeInBits c = src; \
        c89atomic_uint##sizeInBits r = c89atomic_exchange_explicit_##sizeInBits(&b, c, order); \
        if (r == a && b == c) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_exchange_n(sizeInBits, src) \
{ \
    printf("c89atomic_exchange_%d():\n", sizeInBits); \
    c89atomic_test__basic_exchange_explicit(sizeInBits, src, c89atomic_memory_order_relaxed); \
    /*c89atomic_test__basic_exchange_explicit(sizeInBits, src, c89atomic_memory_order_consume);*/ \
    c89atomic_test__basic_exchange_explicit(sizeInBits, src, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_exchange_explicit(sizeInBits, src, c89atomic_memory_order_release); \
    c89atomic_test__basic_exchange_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel); \
    c89atomic_test__basic_exchange_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__exchange(void)
{
    c89atomic_test__basic_exchange_n(8,  123);
    c89atomic_test__basic_exchange_n(16, 1234);
    c89atomic_test__basic_exchange_n(32, 123456);
    c89atomic_test__basic_exchange_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_compare_exchange_strong_explicit(sizeInBits, src, orderSuccess, orderFailure) \
{ \
    printf("    %s, %s ", c89atomic_memory_order_to_string(orderSuccess), c89atomic_memory_order_to_string(orderFailure)); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = OLD_VAL; \
        c89atomic_uint##sizeInBits c = src; \
        c89atomic_bool r = c89atomic_compare_exchange_strong_explicit_##sizeInBits(&a, &b, c, orderSuccess, orderFailure); \
        if (a == c && b == OLD_VAL && r == 1) { \
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */ \
            a = OLD_VAL; \
            b = (c89atomic_uint##sizeInBits)(a + 1); \
            r = c89atomic_compare_exchange_strong_explicit_##sizeInBits(&a, &b, c, orderSuccess, orderFailure); \
            if (a == OLD_VAL && b == OLD_VAL && r == 0) { \
                c89atomic_test_passed(); \
            } else { \
                c89atomic_test_failed(); \
            } \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_compare_exchange_strong_n(sizeInBits, src) \
{ \
    printf("c89atomic_compare_exchange_%d():\n", sizeInBits); \
    c89atomic_test__basic_compare_exchange_strong_explicit(sizeInBits, src, c89atomic_memory_order_relaxed, c89atomic_memory_order_relaxed); \
    c89atomic_test__basic_compare_exchange_strong_explicit(sizeInBits, src, c89atomic_memory_order_consume, c89atomic_memory_order_consume); \
    c89atomic_test__basic_compare_exchange_strong_explicit(sizeInBits, src, c89atomic_memory_order_acquire, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_compare_exchange_strong_explicit(sizeInBits, src, c89atomic_memory_order_release, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_compare_exchange_strong_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_compare_exchange_strong_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__compare_exchange_strong(void)
{
    c89atomic_test__basic_compare_exchange_strong_n(8,  123);
    c89atomic_test__basic_compare_exchange_strong_n(16, 1234);
    c89atomic_test__basic_compare_exchange_strong_n(32, 123456);
    c89atomic_test__basic_compare_exchange_strong_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_compare_exchange_weak_explicit(sizeInBits, src, orderSuccess, orderFailure) \
{ \
    printf("    %s, %s ", c89atomic_memory_order_to_string(orderSuccess), c89atomic_memory_order_to_string(orderFailure)); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = OLD_VAL; \
        c89atomic_uint##sizeInBits c = src; \
        c89atomic_bool r = c89atomic_compare_exchange_weak_explicit_##sizeInBits(&a, &b, c, orderSuccess, orderFailure); \
        if (a == c && b == OLD_VAL && r == 1) { \
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */ \
            a = OLD_VAL; \
            b = (c89atomic_uint##sizeInBits)(a + 1); \
            r = c89atomic_compare_exchange_weak_explicit_##sizeInBits(&a, &b, c, orderSuccess, orderFailure); \
            if (a == OLD_VAL && b == OLD_VAL && r == 0) { \
                c89atomic_test_passed(); \
            } else { \
                c89atomic_test_failed(); \
            } \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_compare_exchange_weak_n(sizeInBits, src) \
{ \
    printf("c89atomic_compare_exchange_weak_%d():\n", sizeInBits); \
    c89atomic_test__basic_compare_exchange_weak_explicit(sizeInBits, src, c89atomic_memory_order_relaxed, c89atomic_memory_order_relaxed); \
    c89atomic_test__basic_compare_exchange_weak_explicit(sizeInBits, src, c89atomic_memory_order_consume, c89atomic_memory_order_consume); \
    c89atomic_test__basic_compare_exchange_weak_explicit(sizeInBits, src, c89atomic_memory_order_acquire, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_compare_exchange_weak_explicit(sizeInBits, src, c89atomic_memory_order_release, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_compare_exchange_weak_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_compare_exchange_weak_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__compare_exchange_weak(void)
{
    c89atomic_test__basic_compare_exchange_weak_n(8,  123);
    c89atomic_test__basic_compare_exchange_weak_n(16, 1234);
    c89atomic_test__basic_compare_exchange_weak_n(32, 123456);
    c89atomic_test__basic_compare_exchange_weak_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_fetch_add_explicit(sizeInBits, src, order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = a; \
        c89atomic_uint##sizeInBits c = src; \
        c89atomic_uint##sizeInBits r = c89atomic_fetch_add_explicit_##sizeInBits(&a, c, order); \
        if (r == b && a == (b + c)) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_fetch_add_n(sizeInBits, src) \
{ \
    printf("c89atomic_fetch_add_%d():\n", sizeInBits); \
    c89atomic_test__basic_fetch_add_explicit(sizeInBits, src, c89atomic_memory_order_relaxed); \
    c89atomic_test__basic_fetch_add_explicit(sizeInBits, src, c89atomic_memory_order_consume); \
    c89atomic_test__basic_fetch_add_explicit(sizeInBits, src, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_fetch_add_explicit(sizeInBits, src, c89atomic_memory_order_release); \
    c89atomic_test__basic_fetch_add_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel); \
    c89atomic_test__basic_fetch_add_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__fetch_add(void)
{
    c89atomic_test__basic_fetch_add_n(8,  123);
    c89atomic_test__basic_fetch_add_n(16, 1234);
    c89atomic_test__basic_fetch_add_n(32, 123456);
    c89atomic_test__basic_fetch_add_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_fetch_sub_explicit(sizeInBits, src, order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = a; \
        c89atomic_uint##sizeInBits c = src; \
        c89atomic_uint##sizeInBits r = c89atomic_fetch_sub_explicit_##sizeInBits(&a, c, order); \
        if (r == b && a == (c89atomic_uint##sizeInBits)(b - c)) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_fetch_sub_n(sizeInBits, src) \
{ \
    printf("c89atomic_fetch_sub_%d():\n", sizeInBits); \
    c89atomic_test__basic_fetch_sub_explicit(sizeInBits, src, c89atomic_memory_order_relaxed); \
    c89atomic_test__basic_fetch_sub_explicit(sizeInBits, src, c89atomic_memory_order_consume); \
    c89atomic_test__basic_fetch_sub_explicit(sizeInBits, src, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_fetch_sub_explicit(sizeInBits, src, c89atomic_memory_order_release); \
    c89atomic_test__basic_fetch_sub_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel); \
    c89atomic_test__basic_fetch_sub_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__fetch_sub(void)
{
    c89atomic_test__basic_fetch_sub_n(8,  123);
    c89atomic_test__basic_fetch_sub_n(16, 1234);
    c89atomic_test__basic_fetch_sub_n(32, 123456);
    c89atomic_test__basic_fetch_sub_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_fetch_or_explicit(sizeInBits, src, order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = a; \
        c89atomic_uint##sizeInBits c = src; \
        c89atomic_uint##sizeInBits r = c89atomic_fetch_or_explicit_##sizeInBits(&a, c, order); \
        if (r == b && a == (c89atomic_uint##sizeInBits)(b | c)) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_fetch_or_n(sizeInBits, src) \
{ \
    printf("c89atomic_fetch_or_%d():\n", sizeInBits); \
    c89atomic_test__basic_fetch_or_explicit(sizeInBits, src, c89atomic_memory_order_relaxed); \
    c89atomic_test__basic_fetch_or_explicit(sizeInBits, src, c89atomic_memory_order_consume); \
    c89atomic_test__basic_fetch_or_explicit(sizeInBits, src, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_fetch_or_explicit(sizeInBits, src, c89atomic_memory_order_release); \
    c89atomic_test__basic_fetch_or_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel); \
    c89atomic_test__basic_fetch_or_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__fetch_or(void)
{
    c89atomic_test__basic_fetch_or_n(8,  123);
    c89atomic_test__basic_fetch_or_n(16, 1234);
    c89atomic_test__basic_fetch_or_n(32, 123456);
    c89atomic_test__basic_fetch_or_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_fetch_xor_explicit(sizeInBits, src, order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = a; \
        c89atomic_uint##sizeInBits c = src; \
        c89atomic_uint##sizeInBits r = c89atomic_fetch_xor_explicit_##sizeInBits(&a, c, order); \
        if (r == b && a == (c89atomic_uint##sizeInBits)(b ^ c)) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_fetch_xor_n(sizeInBits, src) \
{ \
    printf("c89atomic_fetch_xor_%d():\n", sizeInBits); \
    c89atomic_test__basic_fetch_xor_explicit(sizeInBits, src, c89atomic_memory_order_relaxed); \
    c89atomic_test__basic_fetch_xor_explicit(sizeInBits, src, c89atomic_memory_order_consume); \
    c89atomic_test__basic_fetch_xor_explicit(sizeInBits, src, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_fetch_xor_explicit(sizeInBits, src, c89atomic_memory_order_release); \
    c89atomic_test__basic_fetch_xor_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel); \
    c89atomic_test__basic_fetch_xor_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__fetch_xor(void)
{
    c89atomic_test__basic_fetch_xor_n(8,  123);
    c89atomic_test__basic_fetch_xor_n(16, 1234);
    c89atomic_test__basic_fetch_xor_n(32, 123456);
    c89atomic_test__basic_fetch_xor_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_fetch_and_explicit(sizeInBits, src, order) \
{ \
    printf("    %-*s", PRINT_WIDTH, c89atomic_memory_order_to_string(order)); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = a; \
        c89atomic_uint##sizeInBits c = src; \
        c89atomic_uint##sizeInBits r = c89atomic_fetch_and_explicit_##sizeInBits(&a, c, order); \
        if (r == b && a == (c89atomic_uint##sizeInBits)(b & c)) { \
            c89atomic_test_passed(); \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

#define c89atomic_test__basic_fetch_and_n(sizeInBits, src) \
{ \
    printf("c89atomic_fetch_and_%d():\n", sizeInBits); \
    c89atomic_test__basic_fetch_and_explicit(sizeInBits, src, c89atomic_memory_order_relaxed); \
    c89atomic_test__basic_fetch_and_explicit(sizeInBits, src, c89atomic_memory_order_consume); \
    c89atomic_test__basic_fetch_and_explicit(sizeInBits, src, c89atomic_memory_order_acquire); \
    c89atomic_test__basic_fetch_and_explicit(sizeInBits, src, c89atomic_memory_order_release); \
    c89atomic_test__basic_fetch_and_explicit(sizeInBits, src, c89atomic_memory_order_acq_rel); \
    c89atomic_test__basic_fetch_and_explicit(sizeInBits, src, c89atomic_memory_order_seq_cst); \
}

void c89atomic_test__basic__fetch_and(void)
{
    c89atomic_test__basic_fetch_and_n(8,  123);
    c89atomic_test__basic_fetch_and_n(16, 1234);
    c89atomic_test__basic_fetch_and_n(32, 123456);
    c89atomic_test__basic_fetch_and_n(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define c89atomic_test__basic_compare_and_swap_explicit(sizeInBits, src) \
{ \
    printf("c89atomic_compare_and_swap_%d()             ", sizeInBits); printf("%*s", (int)(sizeInBits == 8), ""); \
    { \
        c89atomic_uint##sizeInBits a = OLD_VAL; \
        c89atomic_uint##sizeInBits b = a; \
        c89atomic_uint##sizeInBits c = src; \
        c89atomic_uint##sizeInBits r = c89atomic_compare_and_swap_##sizeInBits(&a, b, c); \
        if (a == c && r == b) { \
            /* Negative case. Expecting a to remain unchanged and the result to not be equal to the expected value. */ \
            a = OLD_VAL; \
            b = (c89atomic_uint##sizeInBits)(a + 1); \
            r = c89atomic_compare_and_swap_##sizeInBits(&a, b, c); \
            if (a == OLD_VAL && r != b) { \
                c89atomic_test_passed(); \
            } else { \
                c89atomic_test_failed(); \
            } \
        } else { \
            c89atomic_test_failed(); \
        } \
    } \
}

void c89atomic_test__basic__compare_and_swap(void)
{
    c89atomic_test__basic_compare_and_swap_explicit(8,  123);
    c89atomic_test__basic_compare_and_swap_explicit(16, 1234);
    c89atomic_test__basic_compare_and_swap_explicit(32, 123456);
    c89atomic_test__basic_compare_and_swap_explicit(64, C89ATOMIC_ULL(123456789012));
    printf("\n");
}


#define C89ATOMIC_CHECK_SIZEOF(type, size) \
{ \
    printf("sizeof(%s)%*.s== %d ", #type, 17 - (int)strlen(#type), "", size); \
    if (sizeof(type) != size) { \
        c89atomic_test_failed(); \
    } else { \
        c89atomic_test_passed(); \
    } \
}

static void c89atomic_test__basic__sizeof(void)
{
    C89ATOMIC_CHECK_SIZEOF(c89atomic_int8,   1);
    C89ATOMIC_CHECK_SIZEOF(c89atomic_uint8,  1);
    C89ATOMIC_CHECK_SIZEOF(c89atomic_int16,  2);
    C89ATOMIC_CHECK_SIZEOF(c89atomic_uint16, 2);
    C89ATOMIC_CHECK_SIZEOF(c89atomic_int32,  4);
    C89ATOMIC_CHECK_SIZEOF(c89atomic_uint32, 4);
    C89ATOMIC_CHECK_SIZEOF(c89atomic_int64,  8);
    C89ATOMIC_CHECK_SIZEOF(c89atomic_uint64, 8);
    printf("\n");
}


int main(int argc, char** argv)
{
    enable_colored_output();

    /* The size of basic types must be valid. If not, the architecture/compiler/platform is not supported. */
    c89atomic_test__basic__sizeof();
    if (g_ErrorCount > 0) {
        printf("Tests cannot continue because the size of one or more basic types are not valid.\n");
        return 1;
    }

    c89atomic_test__basic__flag_test_and_set();
    c89atomic_test__basic__flag_clear();
    c89atomic_test__basic__flag_load();
    c89atomic_test__basic__load();
    c89atomic_test__basic__store();
    c89atomic_test__basic__exchange();
    c89atomic_test__basic__compare_exchange_strong();
    c89atomic_test__basic__compare_exchange_weak();
    c89atomic_test__basic__fetch_add();
    c89atomic_test__basic__fetch_sub();
    c89atomic_test__basic__fetch_or();
    c89atomic_test__basic__fetch_xor();
    c89atomic_test__basic__fetch_and();
    c89atomic_test__basic__compare_and_swap();

    printf("c89atomic_is_lock_free_8  = %s\n", c89atomic_is_lock_free_8(NULL)  ? "\033[32mTRUE\033[0m" : "\033[31mFALSE\033[0m");
    printf("c89atomic_is_lock_free_16 = %s\n", c89atomic_is_lock_free_16(NULL) ? "\033[32mTRUE\033[0m" : "\033[31mFALSE\033[0m");
    printf("c89atomic_is_lock_free_32 = %s\n", c89atomic_is_lock_free_32(NULL) ? "\033[32mTRUE\033[0m" : "\033[31mFALSE\033[0m");
    printf("c89atomic_is_lock_free_64 = %s\n", c89atomic_is_lock_free_64(NULL) ? "\033[32mTRUE\033[0m" : "\033[31mFALSE\033[0m");

    /* Putting these functions here for testing that they compile. */
    c89atomic_thread_fence(c89atomic_memory_order_seq_cst);
    c89atomic_signal_fence(c89atomic_memory_order_seq_cst);

    /* Testing cases where the return value is not used. */
    {
        void* dst = NULL;
        void* src = NULL;
        c89atomic_exchange_ptr(&dst, src);
        c89atomic_compare_exchange_strong_ptr(&dst, &dst, src);
    }

    {
        c89atomic_uint64 dst = 0;
        c89atomic_uint64 src = 1;
        c89atomic_exchange_64(&dst, src);
    }

    /* Testing that some basic integer versions are working without compilation errors. */
    {
        int dst = 0;
        int src = 1;
        int res = c89atomic_exchange_i32(&dst, src);
        (void)res;
    }

    /* Basic floating point tests. */
    {
        float dst = 1.0f;
        float src = 2.0f;
        float res = c89atomic_exchange_f32(&dst, src);
        (void)res;

        res = c89atomic_load_f32(&dst);
        (void)res;
    }

    {
        double dst = 1.0f;
        double src = 2.0f;
        double res = c89atomic_exchange_f64(&dst, src);
        (void)res;

        res = c89atomic_load_f64(&dst);
        (void)res;
    }

    (void)argc;
    (void)argv;

    printf("\n");

    printf("Compiler: ");
    #if defined(__clang__)
        printf("Clang %d.%d\n", __clang_major__, __clang_minor__);
    #elif defined(__GNUC__)
        printf("GCC %d.%d\n", __GNUC__, __GNUC_MINOR__);
    #elif defined(_MSC_VER)
        printf("MSVC %d\n", _MSC_VER);
    #elif defined(__TINYC__)
        printf("TinyCC %d\n", __TINYC__);
    #elif defined(__WATCOMC__)
        printf("Watcom %d.%d\n", __WATCOMC__ / 100, (__WATCOMC__ - (__WATCOMC__ / 100)) / 10);
    #elif defined(__DMC__)
        printf("Digital Mars %d\n", __DMC__);
    #elif defined(__BORLANDC__)
        printf("Borland C++ %d\n", __BORLANDC__);
    #else
        printf("Unknown\n");
    #endif

    printf("Architecture: ");
    #if defined(C89ATOMIC_X64)
        printf("x64\n");
    #endif
    #if defined(C89ATOMIC_X86)
        printf("x86");
        #if defined(__GNUC__)
            #if !defined(__i486__)
                printf(" (i386)\n");
            #elif !defined(__i586__)
                printf(" (i486)\n");
            #elif !defined(__i686__)
                printf(" (i586+)\n");
            #else
                printf("\n");
            #endif
        #elif defined(_M_IX86)
            #if _M_IX86 == 300
                printf(" (i386)\n");
            #elif _M_IX86 == 400
                printf(" (i486)\n");
            #elif _M_IX86 >= 500
                printf(" (i586+)\n");
            #else
                printf("\n");
            #endif
        #else
            printf("\n");
        #endif
    #endif
    #if defined(C89ATOMIC_ARM64)
        printf("ARM64\n");
    #endif
    #if defined(C89ATOMIC_ARM32)
        printf("ARM32\n");
    #endif
    #if defined(C89ATOMIC_PPC64)
        printf("PowerPC64\n");
    #endif
    #if defined(C89ATOMIC_PPC32)
        printf("PowerPC\n");
    #endif

    printf("Code Path: ");
    #if defined(C89ATOMIC_MODERN_GCC)
        printf("GCC __atomic* intrinsics\n");
    #elif defined(C89ATOMIC_LEGACY_GCC)
        printf("GCC __sync* intrinsics\n");
    #elif defined(C89ATOMIC_LEGACY_GCC_ASM)
        printf("GCC inlined assembly\n");
    #elif defined(C89ATOMIC_MODERN_MSVC)
        printf("MSVC _Interlocked* intrinsics\n");
    #elif defined(C89ATOMIC_LEGACY_MSVC)
        printf("MSVC _Interlocked* intrinsics (no lock-free 8/16 bit)\n");
    #elif defined(C89ATOMIC_LEGACY_MSVC_ASM)
        printf("MSVC inlined assembly\n");
    #endif

    if (g_ErrorCount > 0) {
        printf("\033[31m%d test(s) failed.\033[0m\n", g_ErrorCount);
        return 1;
    } else {
        /*printf("\033[32mAll tests passed.\033[0m\n");*/
        return 0;
    }
}

#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
    #pragma GCC diagnostic pop /* -Wlong-long */
#endif
