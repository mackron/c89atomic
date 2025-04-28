/*
Tests basic logic of all atomic functions. Does not test atomicity.
*/
#include <stdio.h>
#include "../c89atomic.h"


/* Need to use a non-0 value for the old value to test byte ordering stuff properly. */
#define OLD_VAL 42


void c89atomic_test__basic__flag_test_and_set(void)
{
    printf("c89atomic_flag_test_and_set():\n");

    printf("    c89atomic_flag_test_and_set:          ");
    {
        c89atomic_flag a = 0;
        c89atomic_flag b = 0;
        c89atomic_bool r = c89atomic_flag_test_and_set(&b);

        if (a == r && b == 1) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}

void c89atomic_test__basic__flag_clear(void)
{
    printf("c89atomic_flag_clear():\n");

    printf("    c89atomic_flag_clear:                 ");
    {
        c89atomic_flag a = 1;
        c89atomic_flag_clear(&a);

        if (a == 0) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}


void c89atomic_test__basic__load(void)
{
    printf("c89atomic_load_*():\n");

    printf("    c89atomic_load_8:                     ");
    {
        c89atomic_uint8 a = 123;
        c89atomic_uint8 b = c89atomic_load_8(&a);
        if (b == a) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_load_16:                    ");
    {
        c89atomic_uint16 a = 1234;
        c89atomic_uint16 b = c89atomic_load_16(&a);
        if (b == a) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_load_32:                    ");
    {
        c89atomic_uint32 a = 123456;
        c89atomic_uint32 b = c89atomic_load_32(&a);
        if (b == a) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_load_64:                    ");
    {
        c89atomic_uint64 a = 123456789012;
        c89atomic_uint64 b = c89atomic_load_64(&a);
        if (b == a) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}

void c89atomic_test__basic__store(void)
{
    printf("c89atomic_store_*():\n");

    printf("    c89atomic_store_8:                    ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = 123;
        c89atomic_store_8(&a, b);

        if (b == a) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_store_16:                   ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = 1234;
        c89atomic_store_16(&a, b);

        if (b == a) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_store_32:                   ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = 123456;
        c89atomic_store_32(&a, b);

        if (b == a) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_store_64:                   ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = 123456789012;
        c89atomic_store_64(&a, b);

        if (b == a) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}


void c89atomic_test__basic__exchange(void)
{
    printf("c89atomic_exchange_*():\n");

    printf("    c89atomic_exchange_8:                 ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = OLD_VAL;
        c89atomic_uint8 c = 123;
        c89atomic_uint8 r = c89atomic_exchange_8(&b, c);

        if (r == a && c == b) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_exchange_16:                ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = OLD_VAL;
        c89atomic_uint16 c = 1234;
        c89atomic_uint16 r = c89atomic_exchange_16(&b, c);

        if (r == a && c == b) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_exchange_32:                ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = OLD_VAL;
        c89atomic_uint32 c = 123456;
        c89atomic_uint32 r = c89atomic_exchange_32(&b, c);

        if (r == a && c == b) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_exchange_64:                ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = OLD_VAL;
        c89atomic_uint64 c = 123456789012;
        c89atomic_uint64 r = c89atomic_exchange_64(&b, c);

        if (r == a && c == b) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}


void c89atomic_test__basic__compare_exchange_strong(void)
{
    printf("c89atomic_compare_exchange_strong_*():\n");

    printf("    c89atomic_compare_exchange_strong_8:  ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = a;
        c89atomic_uint8 c = 123;
        c89atomic_bool  r = c89atomic_compare_exchange_strong_8(&a, &b, c);

        if (a == c && b == OLD_VAL && r == 1) {
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */
            a = OLD_VAL;
            b = (c89atomic_uint8)(a + 1);
            r = c89atomic_compare_exchange_strong_8(&a, &b, c);
            if (a == OLD_VAL && b == OLD_VAL && r == 0) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_compare_exchange_strong_16: ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = a;
        c89atomic_uint16 c = 1234;
        c89atomic_bool   r = c89atomic_compare_exchange_strong_16(&a, &b, c);

        if (a == c && b == OLD_VAL && r == 1) {
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */
            a = OLD_VAL;
            b = (c89atomic_uint16)(a + 1);
            r = c89atomic_compare_exchange_strong_16(&a, &b, c);
            if (a == OLD_VAL && b == OLD_VAL && r == 0) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_compare_exchange_strong_32: ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = a;
        c89atomic_uint32 c = 123456;
        c89atomic_bool   r = c89atomic_compare_exchange_strong_32(&a, &b, c);

        if (a == c && b == OLD_VAL && r == 1) {
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */
            a = OLD_VAL;
            b = a + 1;
            r = c89atomic_compare_exchange_strong_32(&a, &b, c);
            if (a == OLD_VAL && b == OLD_VAL && r == 0) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_compare_exchange_strong_64: ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = a;
        c89atomic_uint64 c = 123456789012;
        c89atomic_bool   r = c89atomic_compare_exchange_strong_64(&a, &b, c);

        if (a == c && b == OLD_VAL && r == 1) {
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */
            a = OLD_VAL;
            b = a + 1;
            r = c89atomic_compare_exchange_strong_64(&a, &b, c);
            if (a == OLD_VAL && b == OLD_VAL && r == 0) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}


void c89atomic_test__basic__compare_exchange_weak(void)
{
    printf("c89atomic_compare_exchange_weak_*():\n");

    printf("    c89atomic_compare_exchange_weak_8:    ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = a;
        c89atomic_uint8 c = 123;
        c89atomic_bool  r = c89atomic_compare_exchange_weak_8(&a, &b, c);

        if (a == c && b == OLD_VAL && r == 1) {
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */
            a = OLD_VAL;
            b = (c89atomic_uint8)(a + 1);
            r = c89atomic_compare_exchange_weak_8(&a, &b, c);
            if (a == OLD_VAL && b == OLD_VAL && r == 0) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_compare_exchange_weak_16:   ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = a;
        c89atomic_uint16 c = 1234;
        c89atomic_bool   r = c89atomic_compare_exchange_weak_16(&a, &b, c);

        if (a == c && b == OLD_VAL && r == 1) {
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */
            a = OLD_VAL;
            b = (c89atomic_uint16)(a + 1);
            r = c89atomic_compare_exchange_weak_16(&a, &b, c);
            if (a == OLD_VAL && b == OLD_VAL && r == 0) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_compare_exchange_weak_32:   ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = a;
        c89atomic_uint32 c = 123456;
        c89atomic_bool   r = c89atomic_compare_exchange_weak_32(&a, &b, c);

        if (a == c && b == OLD_VAL && r == 1) {
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */
            a = OLD_VAL;
            b = a + 1;
            r = c89atomic_compare_exchange_weak_32(&a, &b, c);
            if (a == OLD_VAL && b == OLD_VAL && r == 0) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_compare_exchange_weak_64:   ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = a;
        c89atomic_uint64 c = 123456789012;
        c89atomic_bool   r = c89atomic_compare_exchange_weak_64(&a, &b, c);

        if (a == c && b == OLD_VAL && r == 1) {
            /* Negative case. Expecting a to remain unchanged, b to be set to OLD_VAL (previous value of a) and the result to be false. */
            a = OLD_VAL;
            b = a + 1;
            r = c89atomic_compare_exchange_weak_64(&a, &b, c);
            if (a == OLD_VAL && b == OLD_VAL && r == 0) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}


void c89atomic_test__basic__fetch_add(void)
{
    printf("c89atomic_fetch_add_*():\n");

    printf("    c89atomic_fetch_add_8:                ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = a;
        c89atomic_uint8 c = 123;
        c89atomic_uint8 r = c89atomic_fetch_add_8(&a, c);

        if (r == b && a == (b + c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_add_16:               ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = a;
        c89atomic_uint16 c = 1234;
        c89atomic_uint16 r = c89atomic_fetch_add_16(&a, c);

        if (r == b && a == (b + c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_add_32:               ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = a;
        c89atomic_uint32 c = 123456;
        c89atomic_uint32 r = c89atomic_fetch_add_32(&a, c);

        if (r == b && a == (b + c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_add_64:               ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = a;
        c89atomic_uint64 c = 123456789012;
        c89atomic_uint64 r = c89atomic_fetch_add_64(&a, c);

        if (r == b && a == (b + c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}

void c89atomic_test__basic__fetch_sub(void)
{
    printf("c89atomic_fetch_sub_*():\n");

    printf("    c89atomic_fetch_sub_8:                ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = a;
        c89atomic_uint8 c = 123;
        c89atomic_uint8 r = c89atomic_fetch_sub_8(&a, c);

        if (r == b && a == (c89atomic_uint8)(b - c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_sub_16:               ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = a;
        c89atomic_uint16 c = 1234;
        c89atomic_uint16 r = c89atomic_fetch_sub_16(&a, c);

        if (r == b && a == (c89atomic_uint16)(b - c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_sub_32:               ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = a;
        c89atomic_uint32 c = 123456;
        c89atomic_uint32 r = c89atomic_fetch_sub_32(&a, c);

        if (r == b && a == (c89atomic_uint32)(b - c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_sub_64:               ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = a;
        c89atomic_uint64 c = 123456789012;
        c89atomic_uint64 r = c89atomic_fetch_sub_64(&a, c);

        if (r == b && a == (c89atomic_uint64)(b - c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}


void c89atomic_test__basic__fetch_or(void)
{
    printf("c89atomic_fetch_or_*():\n");

    printf("    c89atomic_fetch_or_8:                 ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = a;
        c89atomic_uint8 c = 123;
        c89atomic_uint8 r = c89atomic_fetch_or_8(&a, c);

        if (r == b && a == (c89atomic_uint8)(b | c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_or_16:                ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = a;
        c89atomic_uint16 c = 1234;
        c89atomic_uint16 r = c89atomic_fetch_or_16(&a, c);

        if (r == b && a == (c89atomic_uint16)(b | c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_or_32:                ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = a;
        c89atomic_uint32 c = 123456;
        c89atomic_uint32 r = c89atomic_fetch_or_32(&a, c);

        if (r == b && a == (c89atomic_uint32)(b | c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_or_64:                ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = a;
        c89atomic_uint64 c = 123456789012;
        c89atomic_uint64 r = c89atomic_fetch_or_64(&a, c);

        if (r == b && a == (c89atomic_uint64)(b | c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}

void c89atomic_test__basic__fetch_xor(void)
{
    printf("c89atomic_fetch_xor_*():\n");

    printf("    c89atomic_fetch_xor_8:                ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = a;
        c89atomic_uint8 c = 123;
        c89atomic_uint8 r = c89atomic_fetch_xor_8(&a, c);

        if (r == b && a == (c89atomic_uint8)(b ^ c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_xor_16:               ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = a;
        c89atomic_uint16 c = 1234;
        c89atomic_uint16 r = c89atomic_fetch_xor_16(&a, c);

        if (r == b && a == (c89atomic_uint16)(b ^ c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_xor_32:               ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = a;
        c89atomic_uint32 c = 123456;
        c89atomic_uint32 r = c89atomic_fetch_xor_32(&a, c);

        if (r == b && a == (c89atomic_uint32)(b ^ c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_xor_64:               ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = a;
        c89atomic_uint64 c = 123456789012;
        c89atomic_uint64 r = c89atomic_fetch_xor_64(&a, c);

        if (r == b && a == (c89atomic_uint64)(b ^ c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}

void c89atomic_test__basic__fetch_and(void)
{
    printf("c89atomic_fetch_and_*():\n");

    printf("    c89atomic_fetch_and_8:                ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = a;
        c89atomic_uint8 c = 123;
        c89atomic_uint8 r = c89atomic_fetch_and_8(&a, c);

        if (r == b && a == (c89atomic_uint8)(b & c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_and_16:               ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = a;
        c89atomic_uint16 c = 1234;
        c89atomic_uint16 r = c89atomic_fetch_and_16(&a, c);

        if (r == b && a == (c89atomic_uint16)(b & c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_and_32:               ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = a;
        c89atomic_uint32 c = 123456;
        c89atomic_uint32 r = c89atomic_fetch_and_32(&a, c);

        if (r == b && a == (c89atomic_uint32)(b & c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_fetch_and_64:               ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = a;
        c89atomic_uint64 c = 123456789012;
        c89atomic_uint64 r = c89atomic_fetch_and_64(&a, c);

        if (r == b && a == (c89atomic_uint64)(b & c)) {
            printf("PASSED\n");
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}


/* compare_and_swap() */
void c89atomic_test__basic__compare_and_swap(void)
{
    printf("c89atomic_compare_and_swap_*():\n");

    printf("    c89atomic_compare_and_swap_8:         ");
    {
        c89atomic_uint8 a = OLD_VAL;
        c89atomic_uint8 b = a;
        c89atomic_uint8 c = 123;
        c89atomic_uint8 r = c89atomic_compare_and_swap_8(&a, b, c);

        if (a == c && r == b) {
            /* Negative case. Expecting a to remain unchanged and the result to not be equal to the expected value. */
            a = OLD_VAL;
            b = (c89atomic_uint8)(a + 1);
            r = c89atomic_compare_and_swap_8(&a, b, c);
            if (a == OLD_VAL && r != b) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_compare_and_swap_16:        ");
    {
        c89atomic_uint16 a = OLD_VAL;
        c89atomic_uint16 b = a;
        c89atomic_uint16 c = 123;
        c89atomic_uint16 r = c89atomic_compare_and_swap_16(&a, b, c);

        if (a == c && r == b) {
            /* Negative case. Expecting a to remain unchanged and the result to not be equal to the expected value. */
            a = OLD_VAL;
            b = (c89atomic_uint16)(a + 1);
            r = c89atomic_compare_and_swap_16(&a, b, c);
            if (a == OLD_VAL && r != b) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_compare_and_swap_32:        ");
    {
        c89atomic_uint32 a = OLD_VAL;
        c89atomic_uint32 b = a;
        c89atomic_uint32 c = 123;
        c89atomic_uint32 r = c89atomic_compare_and_swap_32(&a, b, c);

        if (a == c && r == b) {
            /* Negative case. Expecting a to remain unchanged and the result to not be equal to the expected value. */
            a = OLD_VAL;
            b = (c89atomic_uint32)(a + 1);
            r = c89atomic_compare_and_swap_32(&a, b, c);
            if (a == OLD_VAL && r != b) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("    c89atomic_compare_and_swap_64:        ");
    {
        c89atomic_uint64 a = OLD_VAL;
        c89atomic_uint64 b = a;
        c89atomic_uint64 c = 123;
        c89atomic_uint64 r = c89atomic_compare_and_swap_64(&a, b, c);

        if (a == c && r == b) {
            /* Negative case. Expecting a to remain unchanged and the result to not be equal to the expected value. */
            a = OLD_VAL;
            b = (c89atomic_uint64)(a + 1);
            r = c89atomic_compare_and_swap_64(&a, b, c);
            if (a == OLD_VAL && r != b) {
                printf("PASSED\n");
            } else {
                printf("FAILED\n");
            }
        } else {
            printf("FAILED\n");
        }
    }

    printf("\n");
}


int main(int argc, char** argv)
{
    c89atomic_test__basic__flag_test_and_set();
    c89atomic_test__basic__flag_clear();
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

    /* Putting these functions here for testing that they compile. */
    (void)c89atomic_is_lock_free_8(NULL);
    (void)c89atomic_is_lock_free_16(NULL);
    (void)c89atomic_is_lock_free_32(NULL);
    (void)c89atomic_is_lock_free_64(NULL);
    c89atomic_compiler_fence();
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
        c89atomic_exchange_i32(&dst, src);
    }

    /* Basic floating point tests. */
    {
        int a;
        float dst = 1.0f;
        float src = 2.0f;
        float res = c89atomic_exchange_f32(&dst, src);
        (void)res;

        res = c89atomic_load_f32(&dst);

        a = 5; (void)a;
    }

    {
        int a;
        double dst = 1.0f;
        double src = 2.0f;
        double res = c89atomic_exchange_f64(&dst, src);
        (void)res;

        res = c89atomic_load_f64(&dst);

        a = 5; (void)a;
    }

    (void)argc;
    (void)argv;

    return 0;
}
