/*
C89 compatible atomics. Choice of public domain or MIT-0. See license statements at the end of this file.

David Reid - mackron@gmail.com
*/

/*
Introduction
============
This library aims to implement an equivalent to the C11 atomics library. It's intended to be used as a way to
enable the use of atomics in a mostly consistent manner to modern C, while still enabling compatibility with
older compilers. This is *not* a drop-in replacement for C11 atomics, but is very similar. Only limited testing
has been done so use at your own risk. I'm happy to accept feedback and pull requests with bug fixes.

The following compilers are supported:

  - Visual Studio from VC6 (Earlier versions may work, but are untested.)
  - GCC starting from 2.7 (Earlier versions lack support for extended inline assembly.)
  - Clang
  - Intel oneAPI (Tested with 2025.0.4. Intel C++ Compiler Classic has not been tested.)
  - TinyCC/TCC (Tested with 0.9.27)
  - OpenWatcom (Tested with 2.0)
  - Digital Mars
  - Borland C++ (Tested with 5.02)

New compilers will use intrinsics. GCC-likes, such as GCC and Clang, will use `__atomic_*` intrinsics through a
pre-processor define and should have no overhead. This uses `__GNUC__` to detect GCC-likes.

Old compilers, or compilers lacking support for intrinsics, will use inlined assembly. There are two inlined
assembly paths: GCC-style (GAS syntax) and MSVC-style. For an old compiler to be supported, it must support one
of these two paths. Note that only 32- and 64-bit x86 is supported for inlined assembly. I have not thouroughly
tested the inlined assembly paths. It passes basics tests, but things like memory ordering may have some issues.
Advice welcome on how to improve this.


Differences With C11
--------------------
For practicality, this is not a drop-in replacement for C11's `stdatomic.h`. Below are the main differences
between c89atomic and stdatomic.

    * All operations require an explicit size which is specified by the name of the function, and only 8-,
      16-, 32- and 64-bit operations are supported. Objects of an arbitrary sizes are not supported.
    * All APIs are namespaced with `c89`.
    * `c89atomic_*` data types are undecorated (there is no `_Atomic` decoration).


Types and Functions
-------------------
The following types and functions are implemented:

+-----------------------------------------+-----------------------------------------------+
| C11 Atomics                             | C89 Atomics                                   |
+-----------------------------------------+-----------------------------------------------+
| #include <stdatomic.h>                  | #include "c89atomic.h"                        |
+-----------------------------------------+-----------------------------------------------+
| memory_order                            | c89atomic_memory_order                        |
|     memory_order_relaxed                |     c89atomic_memory_order_relaxed            |
|     memory_order_consume                |     c89atomic_memory_order_consume            |
|     memory_order_acquire                |     c89atomic_memory_order_acquire            |
|     memory_order_release                |     c89atomic_memory_order_release            |
|     memory_order_acq_rel                |     c89atomic_memory_order_acq_rel            |
|     memory_order_seq_cst                |     c89atomic_memory_order_seq_cst            |
+-----------------------------------------+-----------------------------------------------+
| atomic_flag                             | c89atomic_flag                                |
| atomic_bool                             | c89atomic_bool                                |
| atomic_int8                             | c89atomic_int8                                |
| atomic_uint8                            | c89atomic_uint8                               |
| atomic_int16                            | c89atomic_int16                               |
| atomic_uint16                           | c89atomic_uint16                              |
| atomic_int32                            | c89atomic_int32                               |
| atomic_uint32                           | c89atomic_uint32                              |
| atomic_int64                            | c89atomic_int64                               |
| atomic_uint64                           | c89atomic_uint64                              |
+-----------------------------------------+-----------------------------------------------+
| atomic_flag_test_and_set                | c89atomic_flag_test_and_set                   |
| atomic_flag_test_and_set_explicit       | c89atomic_flag_test_and_set_explicit          |
+-----------------------------------------+-----------------------------------------------+
| atomic_flag_clear                       | c89atomic_flag_clear                          |
| atomic_flag_clear_explicit              | c89atomic_flag_clear_explicit                 |
+-----------------------------------------+-----------------------------------------------+
| atomic_store                            | c89atomic_store_8                             |
| atomic_store_explicit                   | c89atomic_store_16                            |
|                                         | c89atomic_store_32                            |
|                                         | c89atomic_store_64                            |
|                                         | c89atomic_store_explicit_8                    |
|                                         | c89atomic_store_explicit_16                   |
|                                         | c89atomic_store_explicit_32                   |
|                                         | c89atomic_store_explicit_64                   |
+-----------------------------------------+-----------------------------------------------+
| atomic_load                             | c89atomic_load_8                              |
| atomic_load_explicit                    | c89atomic_load_16                             |
|                                         | c89atomic_load_32                             |
|                                         | c89atomic_load_64                             |
|                                         | c89atomic_load_explicit_8                     |
|                                         | c89atomic_load_explicit_16                    |
|                                         | c89atomic_load_explicit_32                    |
|                                         | c89atomic_load_explicit_64                    |
+-----------------------------------------+-----------------------------------------------+
| atomic_exchange                         | c89atomic_exchange_8                          |
| atomic_exchange_explicit                | c89atomic_exchange_16                         |
|                                         | c89atomic_exchange_32                         |
|                                         | c89atomic_exchange_64                         |
|                                         | c89atomic_exchange_explicit_8                 |
|                                         | c89atomic_exchange_explicit_16                |
|                                         | c89atomic_exchange_explicit_32                |
|                                         | c89atomic_exchange_explicit_64                |
+-----------------------------------------+-----------------------------------------------+
| atomic_compare_exchange_weak            | c89atomic_compare_exchange_weak_8             |
| atomic_compare_exchange_weak_explicit   | c89atomic_compare_exchange_weak_16            |
| atomic_compare_exchange_strong          | c89atomic_compare_exchange_weak_32            |
| atomic_compare_exchange_strong_explicit | c89atomic_compare_exchange_weak_64            |
|                                         | c89atomic_compare_exchange_weak_explicit_8    |
|                                         | c89atomic_compare_exchange_weak_explicit_16   |
|                                         | c89atomic_compare_exchange_weak_explicit_32   |
|                                         | c89atomic_compare_exchange_weak_explicit_64   |
|                                         | c89atomic_compare_exchange_strong_8           |
|                                         | c89atomic_compare_exchange_strong_16          |
|                                         | c89atomic_compare_exchange_strong_32          |
|                                         | c89atomic_compare_exchange_strong_64          |
|                                         | c89atomic_compare_exchange_strong_explicit_8  |
|                                         | c89atomic_compare_exchange_strong_explicit_16 |
|                                         | c89atomic_compare_exchange_strong_explicit_32 |
|                                         | c89atomic_compare_exchange_strong_explicit_64 |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_add                        | c89atomic_fetch_add_8                         |
| atomic_fetch_add_explicit               | c89atomic_fetch_add_16                        |
|                                         | c89atomic_fetch_add_32                        |
|                                         | c89atomic_fetch_add_64                        |
|                                         | c89atomic_fetch_add_explicit_8                |
|                                         | c89atomic_fetch_add_explicit_16               |
|                                         | c89atomic_fetch_add_explicit_32               |
|                                         | c89atomic_fetch_add_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_sub                        | c89atomic_fetch_sub_8                         |
| atomic_fetch_sub_explicit               | c89atomic_fetch_sub_16                        |
|                                         | c89atomic_fetch_sub_32                        |
|                                         | c89atomic_fetch_sub_64                        |
|                                         | c89atomic_fetch_sub_explicit_8                |
|                                         | c89atomic_fetch_sub_explicit_16               |
|                                         | c89atomic_fetch_sub_explicit_32               |
|                                         | c89atomic_fetch_sub_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_or                         | c89atomic_fetch_or_8                          |
| atomic_fetch_or_explicit                | c89atomic_fetch_or_16                         |
|                                         | c89atomic_fetch_or_32                         |
|                                         | c89atomic_fetch_or_64                         |
|                                         | c89atomic_fetch_or_explicit_8                 |
|                                         | c89atomic_fetch_or_explicit_16                |
|                                         | c89atomic_fetch_or_explicit_32                |
|                                         | c89atomic_fetch_or_explicit_64                |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_xor                        | c89atomic_fetch_xor_8                         |
| atomic_fetch_xor_explicit               | c89atomic_fetch_xor_16                        |
|                                         | c89atomic_fetch_xor_32                        |
|                                         | c89atomic_fetch_xor_64                        |
|                                         | c89atomic_fetch_xor_explicit_8                |
|                                         | c89atomic_fetch_xor_explicit_16               |
|                                         | c89atomic_fetch_xor_explicit_32               |
|                                         | c89atomic_fetch_xor_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_and                        | c89atomic_fetch_and_8                         |
| atomic_fetch_and_explicit               | c89atomic_fetch_and_16                        |
|                                         | c89atomic_fetch_and_32                        |
|                                         | c89atomic_fetch_and_64                        |
|                                         | c89atomic_fetch_and_explicit_8                |
|                                         | c89atomic_fetch_and_explicit_16               |
|                                         | c89atomic_fetch_and_explicit_32               |
|                                         | c89atomic_fetch_and_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_thread_fence()                   | c89atomic_thread_fence                        |
| atomic_signal_fence()                   | c89atomic_signal_fence                        |
+-----------------------------------------+-----------------------------------------------+
| atomic_is_lock_free                     | c89atomic_is_lock_free_8                      |
|                                         | c89atomic_is_lock_free_16                     |
|                                         | c89atomic_is_lock_free_32                     |
|                                         | c89atomic_is_lock_free_64                     |
+-----------------------------------------+-----------------------------------------------+
| (Not Defined)                           | c89atomic_compare_and_swap_8                  |
|                                         | c89atomic_compare_and_swap_16                 |
|                                         | c89atomic_compare_and_swap_32                 |
|                                         | c89atomic_compare_and_swap_64                 |
|                                         | c89atomic_compare_and_swap_ptr                |
+-----------------------------------------+-----------------------------------------------+
*/

#ifndef c89atomic_h
#define c89atomic_h

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wlong-long"
    #if defined(__clang__)
        #pragma GCC diagnostic ignored "-Wc++11-long-long"
    #endif
#endif

typedef int c89atomic_memory_order;

/* Sized Types */
typedef   signed char           c89atomic_int8;
typedef unsigned char           c89atomic_uint8;
typedef   signed short          c89atomic_int16;
typedef unsigned short          c89atomic_uint16;
typedef   signed int            c89atomic_int32;
typedef unsigned int            c89atomic_uint32;
#if (defined(_MSC_VER) && !defined(__clang__)) || defined(__BORLANDC__)
    typedef   signed __int64    c89atomic_int64;
    typedef unsigned __int64    c89atomic_uint64;
#else
    typedef   signed long long  c89atomic_int64;
    typedef unsigned long long  c89atomic_uint64;
#endif

/*
The boolean data type is 32-bit here because it's the most reliable size for ensuring atomic operations
are lock-free. For example, on Visual Studio 2010 and older, 8-bit and 16-bit atomics are not lock-free
on the 64-bit build because those compilers lack support for intrinsics for those sizes, and since there
is no support for inlined assembly on 64-bit builds in any version of Visual Studio, there's no practical
way to implement a fallback outside of external linkage.
*/
typedef c89atomic_uint32        c89atomic_bool;
/* End Sized Types */


/* Architecture Detection */
#if !defined(C89ATOMIC_64BIT) && !defined(C89ATOMIC_32BIT)
    #ifdef _WIN32
        #ifdef _WIN64
            #define C89ATOMIC_64BIT
        #else
            #define C89ATOMIC_32BIT
        #endif
    #endif
#endif

#if !defined(C89ATOMIC_64BIT) && !defined(C89ATOMIC_32BIT)
    #ifdef __GNUC__
        #ifdef __LP64__
            #define C89ATOMIC_64BIT
        #else
            #define C89ATOMIC_32BIT
        #endif
    #endif
#endif

#if !defined(C89ATOMIC_64BIT) && !defined(C89ATOMIC_32BIT)
#include <stdint.h>
#if INTPTR_MAX == INT64_MAX
#define C89ATOMIC_64BIT
#else
#define C89ATOMIC_32BIT
#endif
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define C89ATOMIC_X64
#elif defined(__i386) || defined(_M_IX86) || defined(__i386__)
#define C89ATOMIC_X86
#elif defined(__arm64) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
#define C89ATOMIC_ARM64
#elif defined(__arm__) || defined(_M_ARM)
#define C89ATOMIC_ARM32
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)
#define C89ATOMIC_PPC64
#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__) || defined(__powerpc) || defined(__ppc) || defined(_ARCH_PPC)
#define C89ATOMIC_PPC32
#endif

#if defined(C89ATOMIC_ARM32) || defined(C89ATOMIC_ARM64)
#define C89ATOMIC_ARM
#endif
/* End Architecture Detection */

/* Inline */
/* We want to encourage the compiler to inline. When adding support for a new compiler, make sure it's handled here. */
#if defined(_MSC_VER)
    #define C89ATOMIC_INLINE __forceinline
#elif defined(__GNUC__)
    /*
    I've had a bug report where GCC is emitting warnings about functions possibly not being inlineable. This warning happens when
    the __attribute__((always_inline)) attribute is defined without an "inline" statement. I think therefore there must be some
    case where "__inline__" is not always defined, thus the compiler emitting these warnings. When using -std=c89 or -ansi on the
    command line, we cannot use the "inline" keyword and instead need to use "__inline__". In an attempt to work around this issue
    I am using "__inline__" only when we're compiling in strict ANSI mode.
    */
    #if defined(__STRICT_ANSI__) || !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
        #define C89ATOMIC_INLINE __inline__ __attribute__((always_inline))
    #else
        #define C89ATOMIC_INLINE inline __attribute__((always_inline))
    #endif
#elif defined(__WATCOMC__) || defined(__DMC__)
    #define C89ATOMIC_INLINE __inline
#else
    #define C89ATOMIC_INLINE
#endif
/* End Inline */


/*
Here is where we need to determine the code path we're going to take. This can get complicated
due to the myriad of different compiler versions and available APIs.

If you need to add support for a new (or old!) compiler, try figuring out which code path is
most appropriate and try to plug it into the code path selection logic below. If none of the
existing code paths are appropriate, you'll need to add a new code path and implement it.
*/
#if !defined(C89ATOMIC_MODERN_MSVC) && \
    !defined(C89ATOMIC_LEGACY_MSVC) && \
    !defined(C89ATOMIC_LEGACY_MSVC_ASM) && \
    !defined(C89ATOMIC_MODERN_GCC) && \
    !defined(C89ATOMIC_LEGACY_GCC) && \
    !defined(C89ATOMIC_LEGACY_GCC_ASM)
    #if defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DMC__) || defined(__BORLANDC__)
        #if (defined(_MSC_VER) && _MSC_VER > 1600)
            /* Visual Studio 2010 and later. This path uses _Interlocked* intrinsics. */
            #define C89ATOMIC_MODERN_MSVC
        #else
            /* Old MSVC, or non-MSVC compilers with support for MSVC-style inlined assembly. */
            #if defined(C89ATOMIC_X64)
                /*
                We cannot use inlined assembly for the 64-bit build because 64-bit inlined assembly is not supported
                by any version of Visual Studio.
                */
                #define C89ATOMIC_LEGACY_MSVC
            #else
                /* This path uses MSVC-style inlined assembly. */
                #define C89ATOMIC_LEGACY_MSVC_ASM
            #endif
        #endif
    #elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))) || defined(__clang__)
        /* Modern GCC-compatible compilers. This path uses __atomic_* intrinsics. */
        #define C89ATOMIC_MODERN_GCC
    #else
        #if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
            /* Legacy GCC-compatible compilers. This path uses __sync_* intrinsics. */
            #define C89ATOMIC_LEGACY_GCC
        #else
            /* Ancient GCC compilers, or non-GCC compilers with support for GCC-style inlined assembly (Gas syntax). */
            #define C89ATOMIC_LEGACY_GCC_ASM
        #endif
    #endif
#endif


/*
This section implements the c89atomic_flag type and the following functions:

    - c89atomic_flag_test_and_set_explicit
    - c89atomic_flag_clear_explicit
    - c89atomic_flag_load_explicit

These functions are mandatory. If they cannot be implemented a compile time error must be thrown.
*/
#if defined(C89ATOMIC_MODERN_MSVC) || defined(C89ATOMIC_LEGACY_MSVC)
    #include <intrin.h>

    #define c89atomic_memory_order_relaxed  1
    #define c89atomic_memory_order_consume  2
    #define c89atomic_memory_order_acquire  3
    #define c89atomic_memory_order_release  4
    #define c89atomic_memory_order_acq_rel  5
    #define c89atomic_memory_order_seq_cst  6

    #define C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, intrin, c89atomicType, msvcType)   \
        c89atomicType result; \
        switch (order) \
        { \
            case c89atomic_memory_order_relaxed: \
            { \
                result = (c89atomicType)intrin##_nf((volatile msvcType*)dst, (msvcType)src); \
            } break; \
            case c89atomic_memory_order_consume: \
            case c89atomic_memory_order_acquire: \
            { \
                result = (c89atomicType)intrin##_acq((volatile msvcType*)dst, (msvcType)src); \
            } break; \
            case c89atomic_memory_order_release: \
            { \
                result = (c89atomicType)intrin##_rel((volatile msvcType*)dst, (msvcType)src); \
            } break; \
            case c89atomic_memory_order_acq_rel: \
            case c89atomic_memory_order_seq_cst: \
            default: \
            { \
                result = (c89atomicType)intrin((volatile msvcType*)dst, (msvcType)src); \
            } break; \
        } \
        return result;

    typedef c89atomic_uint32 c89atomic_flag;

    static C89ATOMIC_INLINE c89atomic_flag c89atomic_flag_test_and_set_explicit(volatile c89atomic_flag* dst, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, 1, order, _InterlockedExchange, c89atomic_flag, long);
        }
        #else
        {
            (void)order;    /* Always using the strongest memory order. */
            return (c89atomic_flag)_InterlockedExchange((volatile long*)dst, (long)1);
        }
        #endif
    }

    static C89ATOMIC_INLINE void c89atomic_flag_clear_explicit(volatile c89atomic_flag* dst, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, 0, order, _InterlockedExchange, c89atomic_flag, long);
        }
        #else
        {
            (void)order;    /* Always using the strongest memory order. */
            _InterlockedExchange((volatile long*)dst, (long)0);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_flag c89atomic_flag_load_explicit(volatile const c89atomic_flag* dst, c89atomic_memory_order order)
    {
        (void)order;
        return (c89atomic_uint32)_InterlockedCompareExchange((volatile long*)dst, 0, 0);
    }
#endif

#if defined(C89ATOMIC_LEGACY_MSVC_ASM)
    #define c89atomic_memory_order_relaxed  1
    #define c89atomic_memory_order_consume  2
    #define c89atomic_memory_order_acquire  3
    #define c89atomic_memory_order_release  4
    #define c89atomic_memory_order_acq_rel  5
    #define c89atomic_memory_order_seq_cst  6

    typedef c89atomic_uint32 c89atomic_flag;

    static C89ATOMIC_INLINE c89atomic_flag c89atomic_flag_test_and_set_explicit(volatile c89atomic_flag* dst, c89atomic_memory_order order)
    {
        c89atomic_flag result = 0;

        (void)order;
        __asm {
            mov ecx, dst
            mov eax, 1
            xchg [ecx], eax
            mov result, eax
        }

        return result;
    }

    static C89ATOMIC_INLINE void c89atomic_flag_clear_explicit(volatile c89atomic_flag* dst, c89atomic_memory_order order)
    {
        if (order == c89atomic_memory_order_relaxed) {
            __asm {
                mov esi, dst
                mov dword ptr [esi], 0
            }
        } else {
            __asm {
                mov esi, dst
                mov eax, 0
                xchg [esi], eax
            }
        }
    }

    static C89ATOMIC_INLINE c89atomic_flag c89atomic_flag_load_explicit(volatile const c89atomic_flag* dst, c89atomic_memory_order order)
    {
        c89atomic_flag result = 0;

        if (order == c89atomic_memory_order_relaxed) {
            __asm {
                mov esi, dst
                mov eax, [esi]
                mov result, eax
            }
        } else if (order <= c89atomic_memory_order_release) {
            __asm {
                mov esi, dst
                mov eax, [esi]
                lock add dword ptr [esp], 0 /* fence. */
                mov result, eax
            }
        } else {
            __asm {
                lock add dword ptr [esp], 0 /* fence. */
                mov esi, dst
                mov eax, [esi]
                mov result, eax
                lock add dword ptr [esp], 0 /* fence. */
            }
        }

        return result;
    }
#endif

#if defined(C89ATOMIC_MODERN_GCC)
    #define c89atomic_memory_order_relaxed                   __ATOMIC_RELAXED
    #define c89atomic_memory_order_consume                   __ATOMIC_CONSUME
    #define c89atomic_memory_order_acquire                   __ATOMIC_ACQUIRE
    #define c89atomic_memory_order_release                   __ATOMIC_RELEASE
    #define c89atomic_memory_order_acq_rel                   __ATOMIC_ACQ_REL
    #define c89atomic_memory_order_seq_cst                   __ATOMIC_SEQ_CST

    typedef c89atomic_uint32 c89atomic_flag;

    #define c89atomic_flag_test_and_set_explicit(dst, order) __atomic_exchange_n(dst, 1, order)
    #define c89atomic_flag_clear_explicit(dst, order)        __atomic_store_n(dst, 0, order)
    #define c89atomic_flag_load_explicit(dst, order)         __atomic_load_n(dst, order)
#endif

#if defined(C89ATOMIC_LEGACY_GCC)
    #define c89atomic_memory_order_relaxed  1
    #define c89atomic_memory_order_consume  2
    #define c89atomic_memory_order_acquire  3
    #define c89atomic_memory_order_release  4
    #define c89atomic_memory_order_acq_rel  5
    #define c89atomic_memory_order_seq_cst  6

    typedef c89atomic_uint32 c89atomic_flag;

    static C89ATOMIC_INLINE c89atomic_flag c89atomic_flag_test_and_set_explicit(volatile c89atomic_flag* dst, c89atomic_memory_order order)
    {
        if (order > c89atomic_memory_order_acquire) {
            __sync_synchronize();
        }

        return __sync_lock_test_and_set(dst, 1);
    }

    static C89ATOMIC_INLINE void c89atomic_flag_clear_explicit(volatile c89atomic_flag* dst, c89atomic_memory_order order)
    {
        if (order > c89atomic_memory_order_release) {
            __sync_synchronize();
        }

        __sync_lock_release(dst);
    }

    static C89ATOMIC_INLINE c89atomic_flag c89atomic_flag_load_explicit(volatile const c89atomic_flag* dst, c89atomic_memory_order order)
    {
        (void)order;
        return __sync_val_compare_and_swap((c89atomic_flag*)dst, 0, 0);
    }
#endif

#if defined(C89ATOMIC_LEGACY_GCC_ASM)
    #define c89atomic_memory_order_relaxed  1
    #define c89atomic_memory_order_consume  2
    #define c89atomic_memory_order_acquire  3
    #define c89atomic_memory_order_release  4
    #define c89atomic_memory_order_acq_rel  5
    #define c89atomic_memory_order_seq_cst  6

    /*
    It's actually kind of confusing as to the best way to implement a memory barrier on x86/64. From my quick research, it looks like
    there's a few options:
        - SFENCE/LFENCE/MFENCE
        - LOCK ADD
        - XCHG (with a memory operand, not two register operands)

    It looks like the SFENCE instruction was added in the Pentium III series, whereas the LFENCE and MFENCE instructions were added in
    the Pentium 4 series. It's not clear how this actually differs to a LOCK-prefixed instruction or an XCHG instruction with a memory
    operand. For simplicity and compatibility, I'm just using a LOCK-prefixed ADD instruction which adds 0 to the value pointed to by
    the ESP register. The use of the ESP register is that it should theoretically have a high likelyhood to be in cache. For now, just
    to keep things simple, this is always doing a full memory barrier which means the `order` parameter is ignored on x86/64.
    */
    #if defined(C89ATOMIC_X86)
        #define c89atomic_thread_fence(order) __asm__ __volatile__("lock; addl $0, (%%esp)" ::: "memory")
    #elif defined(C89ATOMIC_X64)
        #define c89atomic_thread_fence(order) __asm__ __volatile__("lock; addq $0, (%%rsp)" ::: "memory")
    #else
        #error Unsupported architecture.
    #endif


    #define C89ATOMIC_XCHG_GCC_X86(instructionSizeSuffix, result, dst, src) \
        __asm__ __volatile__(                    \
            "xchg"instructionSizeSuffix" %0, %1" \
            : "=r"(result), /* %0 */             \
              "=m"(*dst)    /* %1 */             \
            : "0"(src),     /* %2 */             \
              "m"(*dst)     /* %3 */             \
            : "memory"                           \
        )


    #define C89ATOMIC_LOAD_RELAXED_GCC_X86(instructionSizeSuffix, result, dst) \
        __asm__ __volatile__(                   \
            "mov"instructionSizeSuffix" %1, %0" \
            : "=r"(result) /* %0 */             \
            : "m"(*dst)    /* %1 */             \
        )

    #define C89ATOMIC_LOAD_RELEASE_GCC_X86(instructionSizeSuffix, result, dst) \
        c89atomic_thread_fence(c89atomic_memory_order_release); \
        __asm__ __volatile__(                   \
            "mov"instructionSizeSuffix" %1, %0" \
            : "=r"(result) /* %0 */             \
            : "m"(*dst)    /* %1 */             \
            : "memory"                          \
        )

    #define C89ATOMIC_LOAD_SEQ_CST_GCC_X86(instructionSizeSuffix, result, dst) \
        c89atomic_thread_fence(c89atomic_memory_order_seq_cst); \
        __asm__ __volatile__(                   \
            "mov"instructionSizeSuffix" %1, %0" \
            : "=r"(result) /* %0 */             \
            : "m"(*dst)    /* %1 */             \
            : "memory"                          \
        );                                      \
        c89atomic_thread_fence(c89atomic_memory_order_seq_cst)


    typedef c89atomic_uint32 c89atomic_flag;

    static C89ATOMIC_INLINE c89atomic_flag c89atomic_flag_test_and_set_explicit(volatile c89atomic_flag* dst, c89atomic_memory_order order)
    {
        c89atomic_flag result;
        #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
        {
            (void)order;
            C89ATOMIC_XCHG_GCC_X86("l", result, dst, 1);
        }
        #else
        {
            #error Unsupported architecture.
        }
        #endif
        return result;
    }

    static C89ATOMIC_INLINE void c89atomic_flag_clear_explicit(volatile c89atomic_flag* dst, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
        {
            /* The compiler should optimize this branch away because in practice the order is always specified as a constant. */
            if (order == c89atomic_memory_order_relaxed) {
                __asm__ __volatile__(
                    "movl $0, %0"
                    : "=m"(*dst)    /* %0 */
                );
            } else if (order == c89atomic_memory_order_release) {
                __asm__ __volatile__(
                    "movl $0, %0"
                    : "=m"(*dst)    /* %0 */
                    :
                    : "memory"
                );
            } else {
                c89atomic_flag tmp = 0;
                __asm__ __volatile__(
                    "xchgl %0, %1"
                    : "=r"(tmp),    /* %0 */
                      "=m"(*dst)    /* %1 */
                    : "0"(tmp),     /* %2 */
                      "m"(*dst)     /* %3 */
                    : "memory"
                );
            }
        }
        #else
        {
            #error Unsupported architecture.
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_flag c89atomic_flag_load_explicit(volatile const c89atomic_flag* dst, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
        {
            c89atomic_flag result;

            if (order == c89atomic_memory_order_relaxed) {
                C89ATOMIC_LOAD_RELAXED_GCC_X86("l", result, dst);
            } else if (order <= c89atomic_memory_order_release) {
                C89ATOMIC_LOAD_RELEASE_GCC_X86("l", result, dst);
            } else {
                C89ATOMIC_LOAD_SEQ_CST_GCC_X86("l", result, dst);
            }

            return result;
        }
        #else
        {
            #error Unsupported architecture.
        }
        #endif
    }
#endif

#define c89atomic_flag_test_and_set(dst) c89atomic_flag_test_and_set_explicit(dst, c89atomic_memory_order_acquire)
#define c89atomic_flag_clear(dst)        c89atomic_flag_clear_explicit(dst, c89atomic_memory_order_release)

/*
At this point we should have our c89atomic_flag type. We can now define our spinlock. With this
spinlock, any architecture will be able to implement a full API set.
*/
typedef c89atomic_flag c89atomic_spinlock;

static C89ATOMIC_INLINE void c89atomic_spinlock_lock(volatile c89atomic_spinlock* pSpinlock)
{
    for (;;) {
        if (c89atomic_flag_test_and_set_explicit(pSpinlock, c89atomic_memory_order_acquire) == 0) {
            break;
        }

        while (c89atomic_flag_load_explicit(pSpinlock, c89atomic_memory_order_relaxed) == 1) {
            /* Do nothing. */
        }
    }
}

static C89ATOMIC_INLINE void c89atomic_spinlock_unlock(volatile c89atomic_spinlock* pSpinlock)
{
    c89atomic_flag_clear_explicit(pSpinlock, c89atomic_memory_order_release);
}

extern c89atomic_spinlock c89atomic_global_lock;



/*
We define our c89atomic_is_lock_free functions here where applicable because there's a non-trivial
logic in determining whether or not things are lock-free on a given architecture. I'd rather do this
in one place than repeat it in each code path.

Some code paths have their own implementation of these functions in which case those code paths are
not represented here.
*/
#if defined(C89ATOMIC_MODERN_MSVC) || defined(C89ATOMIC_LEGACY_MSVC) || defined(C89ATOMIC_LEGACY_MSVC_ASM) || defined(C89ATOMIC_LEGACY_GCC) || defined(C89ATOMIC_LEGACY_GCC_ASM)
    #if defined(C89ATOMIC_X64) || (defined(C89ATOMIC_X86) && ((defined(__GNUC__) && defined(__i486__)) || (defined(_M_IX86) && _M_IX86 >= 400)))  /* 400 = i486 */
        #if defined(C89ATOMIC_LEGACY_MSVC) && defined(C89ATOMIC_X64)
            /* 64-bit builds on old MSVC do not have access to 8- and 16- bit atomic intrinsics nor an inline assembly. */
        #else
            #define C89ATOMIC_IS_LOCK_FREE_8  1
            #define C89ATOMIC_IS_LOCK_FREE_16 1
        #endif
        #define C89ATOMIC_IS_LOCK_FREE_32     1
        #if defined(C89ATOMIC_X64) || (defined(C89ATOMIC_X86) && ((defined(__GNUC__) && defined(__i586__)) || (defined(_M_IX86) && _M_IX86 >= 500)))  /* 500 = i586 (Pentium) */
            #define C89ATOMIC_IS_LOCK_FREE_64 1
        #else
            /* 64-bit atomics cannot be lock-free on i486 and below because it lacks CMPXCHG8B. */
        #endif
    #else
        /* Atomics cannot be lock-free on i386 because it lacks CMPXCHG. */
    #endif

    #if defined(C89ATOMIC_ARM32) || defined(C89ATOMIC_ARM64)
        #define C89ATOMIC_IS_LOCK_FREE_8  1
        #define C89ATOMIC_IS_LOCK_FREE_16 1
        #define C89ATOMIC_IS_LOCK_FREE_32 1
        #if defined(C89ATOMIC_ARM64) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
            #define C89ATOMIC_IS_LOCK_FREE_64 1
        #endif
    #endif

    #if defined(C89ATOMIC_PPC32) || defined(C89ATOMIC_PPC64)
        /*
        I've had a report that on GCC 4.2 it's possible for some 8-bit and 16-bit __sync* intrinsics to
        not work correctly. To work around this, I'm going to make them use a spinlock instead. I do not
        know which specific versions of GCC this affects, but it certainly is not happening with GCC 4.9 on
        my PowerPC VM running Debian 8. I'm going to be concervative and lock this down to any version of
        GCC that lacks support for the newer __atomic* intrinsics. The reason for this specific boundary is
        that I'm assuming that with those versions of GCC that support the newer __atomic* intrinsics, the
        __sync* intrinsics are just wrappers around __atomic* and should therefore not have the error.
        */
        #if (defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7))) && !defined(__clang__)
            #define C89ATOMIC_IS_LOCK_FREE_8  0
            #define C89ATOMIC_IS_LOCK_FREE_16 0
        #else
            #define C89ATOMIC_IS_LOCK_FREE_8  1
            #define C89ATOMIC_IS_LOCK_FREE_16 1
        #endif
        #define C89ATOMIC_IS_LOCK_FREE_32     1
        #if defined(C89ATOMIC_PPC64)
            #define C89ATOMIC_IS_LOCK_FREE_64 1
        #endif
    #endif

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_is_lock_free_8(volatile void* ptr)
    {
        (void)ptr;
        #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            return 1;
        #else
            return 0;
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_is_lock_free_16(volatile void* ptr)
    {
        (void)ptr;
        #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            return 1;
        #else
            return 0;
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_is_lock_free_32(volatile void* ptr)
    {
        (void)ptr;
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            return 1;
        #else
            return 0;
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_is_lock_free_64(volatile void* ptr)
    {
        (void)ptr;
        #if defined(C89ATOMIC_IS_LOCK_FREE_64)
            return 1;
        #else
            return 0;
        #endif
    }
#endif


#define C89ATOMIC_COMPARE_AND_SWAP_LOCK(sizeInBits, dst, expected, replacement) \
    c89atomic_uint##sizeInBits result; \
    c89atomic_spinlock_lock(&c89atomic_global_lock); \
    { \
        result = *dst; \
        if (result == expected) { \
            *dst = replacement; \
        } \
    } \
    c89atomic_spinlock_unlock(&c89atomic_global_lock); \
    return result


#define C89ATOMIC_LOAD_EXPLICIT_LOCK(sizeInBits, ptr, order) \
    c89atomic_uint##sizeInBits result; \
    c89atomic_spinlock_lock(&c89atomic_global_lock); \
    { \
        result = *ptr; \
        (void)order; \
    } \
    c89atomic_spinlock_unlock(&c89atomic_global_lock); \
    return result


#define C89ATOMIC_STORE_EXPLICIT_LOCK(sizeInBits, dst, src, order) \
    c89atomic_spinlock_lock(&c89atomic_global_lock); \
    { \
        *dst = src; \
        (void)order; \
    } \
    c89atomic_spinlock_unlock(&c89atomic_global_lock)

#define C89ATOMIC_STORE_EXPLICIT_CAS(sizeInBits, dst, src, order) \
    c89atomic_uint##sizeInBits oldValue; \
    do { \
        oldValue = c89atomic_load_explicit_##sizeInBits(dst, c89atomic_memory_order_relaxed); \
    } while (c89atomic_compare_and_swap_##sizeInBits(dst, oldValue, src) != oldValue); \
    (void)order


#define C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(sizeInBits, dst, src, order) \
    c89atomic_uint##sizeInBits result; \
    c89atomic_spinlock_lock(&c89atomic_global_lock); \
    { \
        result = *dst; \
        *dst = src; \
        (void)order; \
    } \
    c89atomic_spinlock_unlock(&c89atomic_global_lock); \
    return result

#define C89ATOMIC_EXCHANGE_EXPLICIT_CAS(sizeInBits, dst, src, order) \
    c89atomic_uint##sizeInBits oldValue; \
    do { \
        oldValue = c89atomic_load_explicit_##sizeInBits(dst, c89atomic_memory_order_relaxed); \
    } while (c89atomic_compare_and_swap_##sizeInBits(dst, oldValue, src) != oldValue); \
    (void)order; \
    return oldValue


#define C89ATOMIC_FETCH_ADD_LOCK(sizeInBits, dst, src, order) \
    c89atomic_uint##sizeInBits result; \
    c89atomic_spinlock_lock(&c89atomic_global_lock); \
    { \
        result = *dst; \
        *dst += src; \
        (void)order; \
    } \
    c89atomic_spinlock_unlock(&c89atomic_global_lock); \
    return result

#define C89ATOMIC_FETCH_ADD_CAS(sizeInBits, dst, src, order) \
    c89atomic_uint##sizeInBits oldValue; \
    c89atomic_uint##sizeInBits newValue; \
    do { \
        oldValue = c89atomic_load_explicit_##sizeInBits(dst, c89atomic_memory_order_relaxed); \
        newValue = oldValue + src; \
    } while (c89atomic_compare_and_swap_##sizeInBits(dst, oldValue, newValue) != oldValue); \
    (void)order; \
    return oldValue


#define C89ATOMIC_FETCH_AND_CAS(sizeInBits, dst, src, order) \
    c89atomic_uint##sizeInBits oldValue; \
    c89atomic_uint##sizeInBits newValue; \
    do { \
        oldValue = c89atomic_load_explicit_##sizeInBits(dst, c89atomic_memory_order_relaxed); \
        newValue = (c89atomic_uint##sizeInBits)(oldValue & src); \
    } while (c89atomic_compare_and_swap_##sizeInBits(dst, oldValue, newValue) != oldValue); \
    (void)order; \
    return oldValue


#define C89ATOMIC_FETCH_OR_CAS(sizeInBits, dst, src, order) \
    c89atomic_uint##sizeInBits oldValue; \
    c89atomic_uint##sizeInBits newValue; \
    do { \
        oldValue = c89atomic_load_explicit_##sizeInBits(dst, c89atomic_memory_order_relaxed); \
        newValue = (c89atomic_uint##sizeInBits)(oldValue | src); \
    } while (c89atomic_compare_and_swap_##sizeInBits(dst, oldValue, newValue) != oldValue); \
    (void)order; \
    return oldValue


#define C89ATOMIC_FETCH_XOR_CAS(sizeInBits, dst, src, order) \
    c89atomic_uint##sizeInBits oldValue; \
    c89atomic_uint##sizeInBits newValue; \
    do { \
        oldValue = c89atomic_load_explicit_##sizeInBits(dst, c89atomic_memory_order_relaxed); \
        newValue = (c89atomic_uint##sizeInBits)(oldValue ^ src); \
    } while (c89atomic_compare_and_swap_##sizeInBits(dst, oldValue, newValue) != oldValue); \
    (void)order; \
    return oldValue


#if defined(C89ATOMIC_MODERN_MSVC) || defined(C89ATOMIC_LEGACY_MSVC)
    /*
    The modern and legacy MSVC paths are basically the same except that with the legacy path, 8-
    and 16-bit atomics are not lock-free on x86_64.
    */
    #define C89ATOMIC_MSVC_ARM_INTRINSIC_COMPARE_EXCHANGE(ptr, expected, replacement, order, intrin, c89atomicType, msvcType)   \
        c89atomicType result; \
        switch (order) \
        { \
            case c89atomic_memory_order_relaxed: \
            { \
                result = (c89atomicType)intrin##_nf((volatile msvcType*)ptr, (msvcType)expected, (msvcType)replacement); \
            } break; \
            case c89atomic_memory_order_consume: \
            case c89atomic_memory_order_acquire: \
            { \
                result = (c89atomicType)intrin##_acq((volatile msvcType*)ptr, (msvcType)expected, (msvcType)replacement); \
            } break; \
            case c89atomic_memory_order_release: \
            { \
                result = (c89atomicType)intrin##_rel((volatile msvcType*)ptr, (msvcType)expected, (msvcType)replacement); \
            } break; \
            case c89atomic_memory_order_acq_rel: \
            case c89atomic_memory_order_seq_cst: \
            default: \
            { \
                result = (c89atomicType)intrin((volatile msvcType*)ptr, (msvcType)expected, (msvcType)replacement); \
            } break; \
        } \
        return result;


    /* compare_and_swap() */
    #if defined(C89ATOMIC_IS_LOCK_FREE_8)
        #define c89atomic_compare_and_swap_8( dst, expected, replacement) (c89atomic_uint8 )_InterlockedCompareExchange8((volatile char*)dst, (char)replacement, (char)expected)
    #else
        static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_compare_and_swap_8(volatile c89atomic_uint8* dst, c89atomic_uint8 expected, c89atomic_uint8 replacement)
        {
            C89ATOMIC_COMPARE_AND_SWAP_LOCK(8, dst, expected, replacement);
        }
    #endif

    #if defined(C89ATOMIC_IS_LOCK_FREE_16)
        #define c89atomic_compare_and_swap_16(dst, expected, replacement) (c89atomic_uint16)_InterlockedCompareExchange16((volatile short*)dst, (short)replacement, (short)expected)
    #else
        static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_compare_and_swap_16(volatile c89atomic_uint16* dst, c89atomic_uint16 expected, c89atomic_uint16 replacement)
        {
            C89ATOMIC_COMPARE_AND_SWAP_LOCK(16, dst, expected, replacement);
        }
    #endif

    #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        #define c89atomic_compare_and_swap_32(dst, expected, replacement) (c89atomic_uint32)_InterlockedCompareExchange((volatile long*)dst, (long)replacement, (long)expected)
    #else
        static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_compare_and_swap_32(volatile c89atomic_uint32* dst, c89atomic_uint32 expected, c89atomic_uint32 replacement)
        {
            C89ATOMIC_COMPARE_AND_SWAP_LOCK(32, dst, expected, replacement);
        }
    #endif

    #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        #define c89atomic_compare_and_swap_64(dst, expected, replacement) (c89atomic_uint64)_InterlockedCompareExchange64((volatile c89atomic_int64*)dst, (c89atomic_int64)replacement, (c89atomic_int64)expected)
    #else
        static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_compare_and_swap_64(volatile c89atomic_uint64* dst, c89atomic_uint64 expected, c89atomic_uint64 replacement)
        {
            C89ATOMIC_COMPARE_AND_SWAP_LOCK(64, dst, expected, replacement);
        }
    #endif


    /* load() */
    static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_load_explicit_8(volatile const c89atomic_uint8* ptr, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_8)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC_COMPARE_EXCHANGE(ptr, 0, 0, order, _InterlockedCompareExchange8, c89atomic_uint8, char);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return c89atomic_compare_and_swap_8((volatile c89atomic_uint8*)ptr, 0, 0);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_LOAD_EXPLICIT_LOCK(8, ptr, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_load_explicit_16(volatile const c89atomic_uint16* ptr, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_16)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC_COMPARE_EXCHANGE(ptr, 0, 0, order, _InterlockedCompareExchange16, c89atomic_uint16, short);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return c89atomic_compare_and_swap_16((volatile c89atomic_uint16*)ptr, 0, 0);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_LOAD_EXPLICIT_LOCK(16, ptr, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_load_explicit_32(volatile const c89atomic_uint32* ptr, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC_COMPARE_EXCHANGE(ptr, 0, 0, order, _InterlockedCompareExchange, c89atomic_uint32, long);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return c89atomic_compare_and_swap_32((volatile c89atomic_uint32*)ptr, 0, 0);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_LOAD_EXPLICIT_LOCK(32, ptr, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_load_explicit_64(volatile const c89atomic_uint64* ptr, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC_COMPARE_EXCHANGE(ptr, 0, 0, order, _InterlockedCompareExchange64, c89atomic_uint64, long long);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return c89atomic_compare_and_swap_64((volatile c89atomic_uint64*)ptr, 0, 0);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_LOAD_EXPLICIT_LOCK(64, ptr, order);
        }
        #endif
    }


    /* exchange() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_exchange_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_8)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedExchange8, c89atomic_uint8, char);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return (c89atomic_uint8)_InterlockedExchange8((volatile char*)dst, (char)src);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(8, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_exchange_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_16)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedExchange16, c89atomic_uint16, short);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return (c89atomic_uint16)_InterlockedExchange16((volatile short*)dst, (short)src);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(16, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_exchange_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedExchange, c89atomic_uint32, long);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return (c89atomic_uint32)_InterlockedExchange((volatile long*)dst, (long)src);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(32, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_exchange_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_64)
        {
            /* atomic_exchange_explicit_64() must be implemented in terms of atomic_compare_and_swap() on 32-bit builds, no matter the version of Visual Studio. */
            #if defined(C89ATOMIC_32BIT)
            {
                C89ATOMIC_EXCHANGE_EXPLICIT_CAS(64, dst, src, order);
            }
            #else
            {
                #if defined(C89ATOMIC_ARM)
                {
                    C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedExchange64, c89atomic_uint64, long long);
                }
                #else
                {
                    (void)order;    /* Always using the strongest memory order. */
                    return (c89atomic_uint64)_InterlockedExchange64((volatile long long*)dst, (long long)src);
                }
                #endif
            }
            #endif
        }
        #else
        {
            C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(64, dst, src, order);
        }
        #endif
    }


    /* fetch_add() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_add_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_8)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedExchangeAdd8, c89atomic_uint8, char);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return (c89atomic_uint8)_InterlockedExchangeAdd8((volatile char*)dst, (char)src);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(8, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_add_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_16)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedExchangeAdd16, c89atomic_uint16, short);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return (c89atomic_uint16)_InterlockedExchangeAdd16((volatile short*)dst, (short)src);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(16, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_add_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        {
            #if defined(C89ATOMIC_ARM)
            {
                C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedExchangeAdd, c89atomic_uint32, long);
            }
            #else
            {
                (void)order;    /* Always using the strongest memory order. */
                return (c89atomic_uint32)_InterlockedExchangeAdd((volatile long*)dst, (long)src);
            }
            #endif
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(32, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_add_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_64)
        {
            /* c89atomic_fetch_add_explicit_64() must be implemented in terms of atomic_compare_and_swap() on 32-bit builds, no matter the version of Visual Studio. */
            #if defined(C89ATOMIC_32BIT)
            {
                C89ATOMIC_FETCH_ADD_CAS(64, dst, src, order);
            }
            #else
            {
                #if defined(C89ATOMIC_ARM)
                {
                    C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedExchangeAdd64, c89atomic_uint64, long long);
                }
                #else
                {
                    (void)order;    /* Always using the strongest memory order. */
                    return (c89atomic_uint64)_InterlockedExchangeAdd64((volatile long long*)dst, (long long)src);
                }
                #endif
            }
            #endif
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(64, dst, src, order);
        }
        #endif
    }


    /* fetch_sub() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_sub_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        return c89atomic_fetch_add_explicit_8(dst, (c89atomic_uint8)(-(c89atomic_int8)src), order);
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_sub_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        return c89atomic_fetch_add_explicit_16(dst, (c89atomic_uint16)(-(c89atomic_int16)src), order);
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_sub_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        return c89atomic_fetch_add_explicit_32(dst, (c89atomic_uint32)(-(c89atomic_int32)src), order);
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_sub_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        return c89atomic_fetch_add_explicit_64(dst, (c89atomic_uint64)(-(c89atomic_int64)src), order);
    }


    /* fetch_and() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_and_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedAnd8, c89atomic_uint8, char);
        }
        #else
        {
            C89ATOMIC_FETCH_AND_CAS(8, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_and_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedAnd16, c89atomic_uint16, short);
        }
        #else
        {
            C89ATOMIC_FETCH_AND_CAS(16, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_and_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedAnd, c89atomic_uint32, long);
        }
        #else
        {
            C89ATOMIC_FETCH_AND_CAS(32, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_and_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedAnd64, c89atomic_uint64, long long);
        }
        #else
        {
            C89ATOMIC_FETCH_AND_CAS(64, dst, src, order);
        }
        #endif
    }


    /* fetch_or() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_or_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedOr8, c89atomic_uint8, char);
        }
        #else
        {
            C89ATOMIC_FETCH_OR_CAS(8, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_or_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedOr16, c89atomic_uint16, short);
        }
        #else
        {
            C89ATOMIC_FETCH_OR_CAS(16, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_or_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedOr, c89atomic_uint32, long);
        }
        #else
        {
            C89ATOMIC_FETCH_OR_CAS(32, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_or_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedOr64, c89atomic_uint64, long long);
        }
        #else
        {
            C89ATOMIC_FETCH_OR_CAS(64, dst, src, order);
        }
        #endif
    }


    /* fetch_xor() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_xor_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedXor8, c89atomic_uint8, char);
        }
        #else
        {
            C89ATOMIC_FETCH_XOR_CAS(8, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_xor_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedXor16, c89atomic_uint16, short);
        }
        #else
        {
            C89ATOMIC_FETCH_XOR_CAS(16, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_xor_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedXor, c89atomic_uint32, long);
        }
        #else
        {
            C89ATOMIC_FETCH_XOR_CAS(32, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_xor_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_ARM)
        {
            C89ATOMIC_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedXor64, c89atomic_uint64, long long);
        }
        #else
        {
            C89ATOMIC_FETCH_XOR_CAS(64, dst, src, order);
        }
        #endif
    }


    /* atomic_store() is the same as atomic_exchange() but returns void. */
    #define c89atomic_store_explicit_8( dst, src, order) (void)c89atomic_exchange_explicit_8 (dst, src, order)
    #define c89atomic_store_explicit_16(dst, src, order) (void)c89atomic_exchange_explicit_16(dst, src, order)
    #define c89atomic_store_explicit_32(dst, src, order) (void)c89atomic_exchange_explicit_32(dst, src, order)
    #define c89atomic_store_explicit_64(dst, src, order) (void)c89atomic_exchange_explicit_64(dst, src, order)


    /* thread_fence() */
    /* Can't use MemoryBarrier() for this as it require Windows headers which we want to avoid in the header section of c89atomic. */
    #if defined(C89ATOMIC_X64)
        #define c89atomic_thread_fence(order)   __faststorefence(), (void)order
    #elif defined(C89ATOMIC_ARM64)
        #define c89atomic_thread_fence(order)   __dmb(_ARM64_BARRIER_ISH), (void)order
    #else
        static C89ATOMIC_INLINE void c89atomic_thread_fence(c89atomic_memory_order order)
        {
            volatile c89atomic_uint32 barrier = 0;
            c89atomic_fetch_add_explicit_32(&barrier, 0, order);
        }
    #endif  /* C89ATOMIC_X64 */

    /* signal_fence() */
    #define c89atomic_signal_fence(order)   _ReadWriteBarrier(), (void)order
#endif


#if defined(C89ATOMIC_LEGACY_MSVC_ASM)
    /*
    Visual Studio 2003 (_MSC_VER 1300) and earlier have no support for sized atomic operations.
    We'll need to use inlined assembly for these compilers.

    I've also had a report that 8-bit and 16-bit interlocked intrinsics were only added in Visual
    Studio 2010 (_MSC_VER 1600). We'll need to disable these on the 64-bit build because there's
    no way to implement them with inlined assembly since Microsoft has decided to drop support for
    it from their 64-bit compilers.

    To simplify the implementation, any older MSVC compilers targeting 32-bit will use inlined
    assembly for everything. I'm not going to use inlined assembly wholesale for all 32-bit builds
    regardless of the age of the compiler because I don't trust the compiler will optimize the
    inlined assembly properly.

    The inlined assembly path supports MSVC, Digital Mars and OpenWatcom. OpenWatcom is a little
    bit too pedantic with it's warnings. A few notes:

      - The return value of these functions are defined by the AL/AX/EAX/EAX:EDX registers which
        means an explicit return statement is not actually necessary. This is helpful for performance
        reasons because it means we can avoid the cost of a declaring a local variable and moving the
        value in EAX into that variable, only to then return it. However, unfortunately OpenWatcom
        thinks this is a mistake and tries to be helpful by throwing a warning. To work around we're
        going to declare a "result" variable and incur this theoretical cost. Most likely the
        compiler will optimize this away and make it a non-issue.

      - Variables that are assigned within the inline assembly will not be detected as such, and
        OpenWatcom will throw a warning about the variable being used without being assigned. To work
        around this we just initialize our local variables to 0.
    */

    /* atomic_compare_and_swap() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_compare_and_swap_8(volatile c89atomic_uint8* dst, c89atomic_uint8 expected, c89atomic_uint8 replacement)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_8)
        {
            c89atomic_uint8 result = 0;

            __asm {
                mov ecx, dst
                mov al,  expected
                mov dl,  replacement
                lock cmpxchg [ecx], dl  /* Writes to EAX which MSVC will treat as the return value. */
                mov result, al
            }

            return result;
        }
        #else
        {
            C89ATOMIC_COMPARE_AND_SWAP_LOCK(8, dst, expected, replacement);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_compare_and_swap_16(volatile c89atomic_uint16* dst, c89atomic_uint16 expected, c89atomic_uint16 replacement)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_16)
        {
            c89atomic_uint16 result = 0;

            __asm {
                mov ecx, dst
                mov ax,  expected
                mov dx,  replacement
                lock cmpxchg [ecx], dx  /* Writes to EAX which MSVC will treat as the return value. */
                mov result, ax
            }

            return result;
        }
        #else
        {
            C89ATOMIC_COMPARE_AND_SWAP_LOCK(16, dst, expected, replacement);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_compare_and_swap_32(volatile c89atomic_uint32* dst, c89atomic_uint32 expected, c89atomic_uint32 replacement)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        {
            c89atomic_uint32 result = 0;

            __asm {
                mov ecx, dst
                mov eax, expected
                mov edx, replacement
                lock cmpxchg [ecx], edx /* Writes to EAX which MSVC will treat as the return value. */
                mov result, eax
            }

            return result;
        }
        #else
        {
            C89ATOMIC_COMPARE_AND_SWAP_LOCK(32, dst, expected, replacement);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_compare_and_swap_64(volatile c89atomic_uint64* dst, c89atomic_uint64 expected, c89atomic_uint64 replacement)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_64)
        {
            c89atomic_uint32 resultEAX = 0;
            c89atomic_uint32 resultEDX = 0;

            __asm {
                mov esi, dst    /* From Microsoft documentation: "... you don't need to preserve the EAX, EBX, ECX, EDX, ESI, or EDI registers." Choosing ESI since it's the next available one in their list. */
                mov eax, dword ptr expected
                mov edx, dword ptr expected + 4
                mov ebx, dword ptr replacement
                mov ecx, dword ptr replacement + 4
                lock cmpxchg8b qword ptr [esi]  /* Writes to EAX:EDX which MSVC will treat as the return value. */
                mov resultEAX, eax
                mov resultEDX, edx
            }

            return ((c89atomic_uint64)resultEDX << 32) | resultEAX;
        }
        #else
        {
            C89ATOMIC_COMPARE_AND_SWAP_LOCK(64, dst, expected, replacement);
        }
        #endif
    }


    /* load_explicit() */
    static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_load_explicit_8(volatile const c89atomic_uint8* dst, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_8)
        {
            c89atomic_uint8 result = 0;

            if (order == c89atomic_memory_order_relaxed) {
                __asm {
                    mov esi, dst
                    mov al, [esi]
                    mov result, al
                }
            } else if (order <= c89atomic_memory_order_release) {
                __asm {
                    mov esi, dst
                    mov al, [esi]
                    lock add dword ptr [esp], 0 /* fence. */
                    mov result, al
                }
            } else {
                __asm {
                    lock add dword ptr [esp], 0 /* fence. */
                    mov esi, dst
                    mov al, [esi]
                    mov result, al
                    lock add dword ptr [esp], 0 /* fence. */
                }
            }

            return result;
        }
        #else
        {
            C89ATOMIC_LOAD_EXPLICIT_LOCK(8, dst, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_load_explicit_16(volatile const c89atomic_uint16* dst, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_16)
        {
            c89atomic_uint16 result = 0;

            if (order == c89atomic_memory_order_relaxed) {
                __asm {
                    mov esi, dst
                    mov ax, [esi]
                    mov result, ax
                }
            } else if (order <= c89atomic_memory_order_release) {
                __asm {
                    mov esi, dst
                    mov ax, [esi]
                    lock add dword ptr [esp], 0 /* fence. */
                    mov result, ax
                }
            } else {
                __asm {
                    lock add dword ptr [esp], 0 /* fence. */
                    mov esi, dst
                    mov ax, [esi]
                    mov result, ax
                    lock add dword ptr [esp], 0 /* fence. */
                }
            }

            return result;
        }
        #else
        {
            C89ATOMIC_LOAD_EXPLICIT_LOCK(16, dst, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_load_explicit_32(volatile const c89atomic_uint32* dst, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        {
            c89atomic_uint32 result = 0;

            if (order == c89atomic_memory_order_relaxed) {
                __asm {
                    mov esi, dst
                    mov eax, [esi]
                    mov result, eax
                }
            } else if (order <= c89atomic_memory_order_release) {
                __asm {
                    mov esi, dst
                    mov eax, [esi]
                    lock add dword ptr [esp], 0 /* fence. */
                    mov result, eax
                }
            } else {
                __asm {
                    lock add dword ptr [esp], 0 /* fence. */
                    mov esi, dst
                    mov eax, [esi]
                    mov result, eax
                    lock add dword ptr [esp], 0 /* fence. */
                }
            }

            return result;
        }
        #else
        {
            C89ATOMIC_LOAD_EXPLICIT_LOCK(32, dst, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_load_explicit_64(volatile const c89atomic_uint64* dst, c89atomic_memory_order order)
    {
        (void)order;
        return c89atomic_compare_and_swap_64((volatile c89atomic_uint64*)dst, 0, 0);
    }


    /* atomic_store() */
    static C89ATOMIC_INLINE void __stdcall c89atomic_store_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        /* With memory_order_relaxed we can just do a straight mov with x86. */
        if (order == c89atomic_memory_order_relaxed) {
            __asm {
                mov esi, dst
                mov al, src
                mov [esi], al
            }
        } else {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            {
                __asm {
                    mov esi, dst
                    mov al, src
                    xchg [esi], al
                }
            }
            #else
            {
                C89ATOMIC_STORE_EXPLICIT_LOCK(8, dst, src, order);
            }
            #endif
        }
    }

    static C89ATOMIC_INLINE void __stdcall c89atomic_store_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        /* With memory_order_relaxed we can just do a straight mov with x86. */
        if (order == c89atomic_memory_order_relaxed) {
            __asm {
                mov esi, dst
                mov ax, src
                mov [esi], ax
            }
        } else {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            {
                __asm {
                    mov esi, dst
                    mov ax, src
                    xchg [esi], ax
                }
            }
            #else
            {
                C89ATOMIC_STORE_EXPLICIT_LOCK(16, dst, src, order);
            }
            #endif
        }
    }

    static C89ATOMIC_INLINE void __stdcall c89atomic_store_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        /* With memory_order_relaxed we can just do a straight mov with x86. */
        if (order == c89atomic_memory_order_relaxed) {
            __asm {
                mov esi, dst
                mov eax, src
                mov [esi], eax
            }
        } else {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            {
                __asm {
                    mov esi, dst
                    mov eax, src
                    xchg [esi], eax
                }
            }
            #else
            {
                C89ATOMIC_STORE_EXPLICIT_LOCK(32, dst, src, order);
            }
            #endif
        }
    }

    static C89ATOMIC_INLINE void __stdcall c89atomic_store_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_64)
        {
            C89ATOMIC_STORE_EXPLICIT_CAS(64, dst, src, order);
        }
        #else
        {
            C89ATOMIC_STORE_EXPLICIT_LOCK(64, dst, src, order);
        }
        #endif
    }


    /* exchange_explicit() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_exchange_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_8)
        {
            c89atomic_uint8 result = 0;

            (void)order;
            __asm {
                mov ecx, dst
                mov al,  src
                lock xchg [ecx], al
                mov result, al
            }

            return result;
        }
        #else
        {
            C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(8, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_exchange_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_16)
        {
            c89atomic_uint16 result = 0;

            (void)order;
            __asm {
                mov ecx, dst
                mov ax,  src
                lock xchg [ecx], ax
                mov result, ax
            }

            return result;
        }
        #else
        {
            C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(16, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_exchange_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        {
            c89atomic_uint32 result = 0;

            (void)order;
            __asm {
                mov ecx, dst
                mov eax, src
                xchg [ecx], eax
                mov result, eax
            }

            return result;
        }
        #else
        {
            C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(32, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_exchange_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_64)
        {
            C89ATOMIC_EXCHANGE_EXPLICIT_CAS(64, dst, src, order);
        }
        #else
        {
            C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(64, dst, src, order);
        }
        #endif
    }


    /* atomic_fetch_add */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_add_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_8)
        {
            c89atomic_uint8 result = 0;

            (void)order;
            __asm {
                mov ecx, dst
                mov al,  src
                lock xadd [ecx], al
                mov result, al
            }

            return result;
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(8, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_add_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_16)
        {
            c89atomic_uint16 result = 0;

            (void)order;
            __asm {
                mov ecx, dst
                mov ax,  src
                lock xadd [ecx], ax
                mov result, ax
            }

            return result;
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(16, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_add_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        {
            c89atomic_uint32 result = 0;

            (void)order;
            __asm {
                mov ecx, dst
                mov eax, src
                lock xadd [ecx], eax
                mov result, eax
            }

            return result;
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(32, dst, src, order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_add_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_64)
        {
            C89ATOMIC_FETCH_ADD_CAS(64, dst, src, order);
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(64, dst, src, order);
        }
        #endif
    }


    /* fetch_sub() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_sub_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_8)
        {
            c89atomic_uint8 result = 0;

            (void)order;
            __asm {
                mov ecx, dst
                mov al,  src
                neg al
                lock xadd [ecx], al
                mov result, al
            }

            return result;
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(8, dst, (c89atomic_uint8)(-(c89atomic_int8)src), order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_sub_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_16)
        {
            c89atomic_uint16 result = 0;

            (void)order;
            __asm {
                mov ecx, dst
                mov ax,  src
                neg ax
                lock xadd [ecx], ax
                mov result, ax
            }

            return result;
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(16, dst, (c89atomic_uint16)(-(c89atomic_int16)src), order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_sub_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        #if defined(C89ATOMIC_IS_LOCK_FREE_32)
        {
            c89atomic_uint32 result = 0;

            (void)order;
            __asm {
                mov ecx, dst
                mov eax, src
                neg eax
                lock xadd [ecx], eax
                mov result, eax
            }

            return result;
        }
        #else
        {
            C89ATOMIC_FETCH_ADD_LOCK(32, dst, (c89atomic_uint32)(-(c89atomic_int32)src), order);
        }
        #endif
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_sub_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_ADD_CAS(64, dst, (c89atomic_uint64)(-(c89atomic_int64)src), order);
    }


    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_and_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_AND_CAS(8, dst, src, order);
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_and_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_AND_CAS(16, dst, src, order);
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_and_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_AND_CAS(32, dst, src, order);
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_and_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_AND_CAS(64, dst, src, order);
    }


    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_or_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_OR_CAS(8, dst, src, order);
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_or_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_OR_CAS(16, dst, src, order);
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_or_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_OR_CAS(32, dst, src, order);
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_or_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_OR_CAS(64, dst, src, order);
    }


    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_xor_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_XOR_CAS(8, dst, src, order);
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_xor_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_XOR_CAS(16, dst, src, order);
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_xor_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_XOR_CAS(32, dst, src, order);
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_xor_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        C89ATOMIC_FETCH_XOR_CAS(64, dst, src, order);
    }


    /* atomic_thread_fence */
    static C89ATOMIC_INLINE void __stdcall c89atomic_thread_fence(c89atomic_memory_order order)
    {
        (void)order;
        __asm {
            lock add dword ptr [esp], 0
        }
    }

    /* atomic_signal_fence */
    #define c89atomic_signal_fence(order) __asm {}; (void)order
#endif

#if defined(C89ATOMIC_MODERN_GCC)
    #define C89ATOMIC_HAS_NATIVE_COMPARE_EXCHANGE

    #define c89atomic_thread_fence(order)                           __atomic_thread_fence(order)
    #define c89atomic_signal_fence(order)                           __atomic_signal_fence(order)

    #define c89atomic_is_lock_free_8(ptr)                           __atomic_is_lock_free(1, ptr)
    #define c89atomic_is_lock_free_16(ptr)                          __atomic_is_lock_free(2, ptr)
    #define c89atomic_is_lock_free_32(ptr)                          __atomic_is_lock_free(4, ptr)
    #define c89atomic_is_lock_free_64(ptr)                          __atomic_is_lock_free(8, ptr)

    #define c89atomic_store_explicit_8( dst, src, order)            __atomic_store_n(dst, src, order)
    #define c89atomic_store_explicit_16(dst, src, order)            __atomic_store_n(dst, src, order)
    #define c89atomic_store_explicit_32(dst, src, order)            __atomic_store_n(dst, src, order)
    #define c89atomic_store_explicit_64(dst, src, order)            __atomic_store_n(dst, src, order)

    #define c89atomic_load_explicit_8( dst, order)                  __atomic_load_n(dst, order)
    #define c89atomic_load_explicit_16(dst, order)                  __atomic_load_n(dst, order)
    #define c89atomic_load_explicit_32(dst, order)                  __atomic_load_n(dst, order)
    #define c89atomic_load_explicit_64(dst, order)                  __atomic_load_n(dst, order)

    #define c89atomic_exchange_explicit_8( dst, src, order)         __atomic_exchange_n(dst, src, order)
    #define c89atomic_exchange_explicit_16(dst, src, order)         __atomic_exchange_n(dst, src, order)
    #define c89atomic_exchange_explicit_32(dst, src, order)         __atomic_exchange_n(dst, src, order)
    #define c89atomic_exchange_explicit_64(dst, src, order)         __atomic_exchange_n(dst, src, order)

    #define c89atomic_compare_exchange_strong_explicit_8( dst, expected, replacement, successOrder, failureOrder)   __atomic_compare_exchange_n(dst, expected, replacement, 0, successOrder, failureOrder)
    #define c89atomic_compare_exchange_strong_explicit_16(dst, expected, replacement, successOrder, failureOrder)   __atomic_compare_exchange_n(dst, expected, replacement, 0, successOrder, failureOrder)
    #define c89atomic_compare_exchange_strong_explicit_32(dst, expected, replacement, successOrder, failureOrder)   __atomic_compare_exchange_n(dst, expected, replacement, 0, successOrder, failureOrder)
    #define c89atomic_compare_exchange_strong_explicit_64(dst, expected, replacement, successOrder, failureOrder)   __atomic_compare_exchange_n(dst, expected, replacement, 0, successOrder, failureOrder)

    #define c89atomic_compare_exchange_weak_explicit_8( dst, expected, replacement, successOrder, failureOrder)     __atomic_compare_exchange_n(dst, expected, replacement, 1, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_16(dst, expected, replacement, successOrder, failureOrder)     __atomic_compare_exchange_n(dst, expected, replacement, 1, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_32(dst, expected, replacement, successOrder, failureOrder)     __atomic_compare_exchange_n(dst, expected, replacement, 1, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_64(dst, expected, replacement, successOrder, failureOrder)     __atomic_compare_exchange_n(dst, expected, replacement, 1, successOrder, failureOrder)

    #define c89atomic_fetch_add_explicit_8( dst, src, order)        __atomic_fetch_add(dst, src, order)
    #define c89atomic_fetch_add_explicit_16(dst, src, order)        __atomic_fetch_add(dst, src, order)
    #define c89atomic_fetch_add_explicit_32(dst, src, order)        __atomic_fetch_add(dst, src, order)
    #define c89atomic_fetch_add_explicit_64(dst, src, order)        __atomic_fetch_add(dst, src, order)

    #define c89atomic_fetch_sub_explicit_8( dst, src, order)        __atomic_fetch_sub(dst, src, order)
    #define c89atomic_fetch_sub_explicit_16(dst, src, order)        __atomic_fetch_sub(dst, src, order)
    #define c89atomic_fetch_sub_explicit_32(dst, src, order)        __atomic_fetch_sub(dst, src, order)
    #define c89atomic_fetch_sub_explicit_64(dst, src, order)        __atomic_fetch_sub(dst, src, order)

    #define c89atomic_fetch_or_explicit_8( dst, src, order)         __atomic_fetch_or(dst, src, order)
    #define c89atomic_fetch_or_explicit_16(dst, src, order)         __atomic_fetch_or(dst, src, order)
    #define c89atomic_fetch_or_explicit_32(dst, src, order)         __atomic_fetch_or(dst, src, order)
    #define c89atomic_fetch_or_explicit_64(dst, src, order)         __atomic_fetch_or(dst, src, order)

    #define c89atomic_fetch_xor_explicit_8( dst, src, order)        __atomic_fetch_xor(dst, src, order)
    #define c89atomic_fetch_xor_explicit_16(dst, src, order)        __atomic_fetch_xor(dst, src, order)
    #define c89atomic_fetch_xor_explicit_32(dst, src, order)        __atomic_fetch_xor(dst, src, order)
    #define c89atomic_fetch_xor_explicit_64(dst, src, order)        __atomic_fetch_xor(dst, src, order)

    #define c89atomic_fetch_and_explicit_8( dst, src, order)        __atomic_fetch_and(dst, src, order)
    #define c89atomic_fetch_and_explicit_16(dst, src, order)        __atomic_fetch_and(dst, src, order)
    #define c89atomic_fetch_and_explicit_32(dst, src, order)        __atomic_fetch_and(dst, src, order)
    #define c89atomic_fetch_and_explicit_64(dst, src, order)        __atomic_fetch_and(dst, src, order)

    /* CAS needs to be implemented as a function because _atomic_compare_exchange_n() needs to take the address of the expected value. */
    static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_compare_and_swap_8(volatile c89atomic_uint8* dst, c89atomic_uint8 expected, c89atomic_uint8 replacement)
    {
        __atomic_compare_exchange_n(dst, &expected, replacement, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
        return expected;
    }

    static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_compare_and_swap_16(volatile c89atomic_uint16* dst, c89atomic_uint16 expected, c89atomic_uint16 replacement)
    {
        __atomic_compare_exchange_n(dst, &expected, replacement, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
        return expected;
    }

    static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_compare_and_swap_32(volatile c89atomic_uint32* dst, c89atomic_uint32 expected, c89atomic_uint32 replacement)
    {
        __atomic_compare_exchange_n(dst, &expected, replacement, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
        return expected;
    }

    /*
    The compare_and_swap function below will result in the following warning when compiling via Android Studio
    which uses Clang:

        warning: misaligned atomic operation may incur significant performance penalty [-Watomic-alignment]

    What I think is happening is that Clang is looking at this function in a bubble and not considering the
    broader context in which the function is being used, which is surprising to me considering the function is
    marked as force-inlined. So I think this warning is being reported incorrectly.

    I've only seen this warning with compare_and_swap_64(). If this is happening with compare_and_swap_32/16/8(),
    just move the pragmas up to encapsulate the affected functions.
    */
    #if defined(__clang__)
        #pragma clang diagnostic push
        #if __clang_major__ >= 8    /* <-- I don't know the exact version of Clang -Watomic-alignment was introduced, but it's definitely available in version 8. With my version of TrueOS2 I have Clang version 6 installed, but I'm getting a warning about an unknown warning group.  */
            #pragma clang diagnostic ignored "-Watomic-alignment"
        #endif
    #endif
    static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_compare_and_swap_64(volatile c89atomic_uint64* dst, c89atomic_uint64 expected, c89atomic_uint64 replacement)
    {
        __atomic_compare_exchange_n(dst, &expected, replacement, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
        return expected;
    }
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif
#endif

#if defined(C89ATOMIC_LEGACY_GCC) || defined(C89ATOMIC_LEGACY_GCC_ASM)
    #define c89atomic_signal_fence(order)   __asm__ __volatile__("":::"memory")

    #if defined(C89ATOMIC_LEGACY_GCC)
        /* Legacy GCC atomic built-ins. Everything is a full memory barrier. */
        #define c89atomic_thread_fence(order) __sync_synchronize(), (void)order


        /* compare_and_swap() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_compare_and_swap_8(volatile c89atomic_uint8* dst, c89atomic_uint8 expected, c89atomic_uint8 replacement)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            {
                return __sync_val_compare_and_swap(dst, expected, replacement);
            }
            #else
            {
                C89ATOMIC_COMPARE_AND_SWAP_LOCK(8, dst, expected, replacement);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_compare_and_swap_16(volatile c89atomic_uint16* dst, c89atomic_uint16 expected, c89atomic_uint16 replacement)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            {
                return __sync_val_compare_and_swap(dst, expected, replacement);
            }
            #else
            {
                C89ATOMIC_COMPARE_AND_SWAP_LOCK(16, dst, expected, replacement);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_compare_and_swap_32(volatile c89atomic_uint32* dst, c89atomic_uint32 expected, c89atomic_uint32 replacement)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            {
                return __sync_val_compare_and_swap(dst, expected, replacement);
            }
            #else
            {
                C89ATOMIC_COMPARE_AND_SWAP_LOCK(32, dst, expected, replacement);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_compare_and_swap_64(volatile c89atomic_uint64* dst, c89atomic_uint64 expected, c89atomic_uint64 replacement)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64)
            {
                return __sync_val_compare_and_swap(dst, expected, replacement);
            }
            #else
            {
                C89ATOMIC_COMPARE_AND_SWAP_LOCK(64, dst, expected, replacement);
            }
            #endif
        }


        /* Atomic loads can be implemented in terms of a compare-and-swap. Need to implement as functions to silence warnings about `order` being unused. */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_load_explicit_8(volatile const c89atomic_uint8* ptr, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            {
                (void)order;    /* Always using the strongest memory order. */
                return c89atomic_compare_and_swap_8((c89atomic_uint8*)ptr, 0, 0);
            }
            #else
            {
                C89ATOMIC_LOAD_EXPLICIT_LOCK(8, ptr, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_load_explicit_16(volatile const c89atomic_uint16* ptr, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            {
                (void)order;    /* Always using the strongest memory order. */
                return c89atomic_compare_and_swap_16((c89atomic_uint16*)ptr, 0, 0);
            }
            #else
            {
                C89ATOMIC_LOAD_EXPLICIT_LOCK(16, ptr, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_load_explicit_32(volatile const c89atomic_uint32* ptr, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            {
                (void)order;    /* Always using the strongest memory order. */
                return c89atomic_compare_and_swap_32((c89atomic_uint32*)ptr, 0, 0);
            }
            #else
            {
                C89ATOMIC_LOAD_EXPLICIT_LOCK(32, ptr, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_load_explicit_64(volatile const c89atomic_uint64* ptr, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64)
            {
                (void)order;    /* Always using the strongest memory order. */
                return c89atomic_compare_and_swap_64((c89atomic_uint64*)ptr, 0, 0);
            }
            #else
            {
                C89ATOMIC_LOAD_EXPLICIT_LOCK(64, ptr, order);
            }
            #endif
        }



        /* exchange() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_exchange_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            {
                if (order > c89atomic_memory_order_acquire) {
                    __sync_synchronize();
                }

                return __sync_lock_test_and_set(dst, src);
            }
            #else
            {
                C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(8, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_exchange_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            {
                if (order > c89atomic_memory_order_acquire) {
                    __sync_synchronize();
                }

                return __sync_lock_test_and_set(dst, src);
            }
            #else
            {
                C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(16, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_exchange_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            {
                if (order > c89atomic_memory_order_acquire) {
                    __sync_synchronize();
                }

                return __sync_lock_test_and_set(dst, src);
            }
            #else
            {
                C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(32, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_exchange_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64)
            {
                if (order > c89atomic_memory_order_acquire) {
                    __sync_synchronize();
                }

                return __sync_lock_test_and_set(dst, src);
            }
            #else
            {
                C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(64, dst, src, order);
            }
            #endif
        }


        /* store() */
        #define c89atomic_store_explicit_8( dst, src, order) (void)c89atomic_exchange_explicit_8 (dst, src, order)
        #define c89atomic_store_explicit_16(dst, src, order) (void)c89atomic_exchange_explicit_16(dst, src, order)
        #define c89atomic_store_explicit_32(dst, src, order) (void)c89atomic_exchange_explicit_32(dst, src, order)
        #define c89atomic_store_explicit_64(dst, src, order) (void)c89atomic_exchange_explicit_64(dst, src, order)


        /* fetch_add() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_add_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            {
                (void)order;
                return __sync_fetch_and_add(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(8, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_add_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            {
                (void)order;
                return __sync_fetch_and_add(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(16, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_add_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            {
                (void)order;
                return __sync_fetch_and_add(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(32, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_add_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64)
            {
                (void)order;
                return __sync_fetch_and_add(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(64, dst, src, order);
            }
            #endif
        }


        /* fetch_sub() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_sub_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            {
                (void)order;
                return __sync_fetch_and_sub(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(8, dst, (c89atomic_uint8)(-(c89atomic_int8)src), order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_sub_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            {
                (void)order;
                return __sync_fetch_and_sub(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(16, dst, (c89atomic_uint16)(-(c89atomic_int16)src), order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_sub_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            {
                (void)order;
                return __sync_fetch_and_sub(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(32, dst, (c89atomic_uint32)(-(c89atomic_int32)src), order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_sub_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64)
            {
                (void)order;
                return __sync_fetch_and_sub(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(64, dst, (c89atomic_uint64)(-(c89atomic_int64)src), order);
            }
            #endif
        }


        /* fetch_and() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_and_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            {
                (void)order;
                return __sync_fetch_and_and(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_AND_CAS(8, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_and_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            {
                (void)order;
                return __sync_fetch_and_and(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_AND_CAS(16, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_and_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            {
                (void)order;
                return __sync_fetch_and_and(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_AND_CAS(32, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_and_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64)
            {
                (void)order;
                return __sync_fetch_and_and(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_AND_CAS(64, dst, src, order);
            }
            #endif
        }


        /* fetch_or() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_or_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            {
                (void)order;
                return __sync_fetch_and_or(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_OR_CAS(8, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_or_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            {
                (void)order;
                return __sync_fetch_and_or(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_OR_CAS(16, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_or_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            {
                (void)order;
                return __sync_fetch_and_or(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_OR_CAS(32, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_or_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64)
            {
                (void)order;
                return __sync_fetch_and_or(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_OR_CAS(64, dst, src, order);
            }
            #endif
        }


        /* fetch_xor() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_xor_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8)
            {
                (void)order;
                return __sync_fetch_and_xor(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_XOR_CAS(8, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_xor_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16)
            {
                (void)order;
                return __sync_fetch_and_xor(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_XOR_CAS(16, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_xor_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32)
            {
                (void)order;
                return __sync_fetch_and_xor(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_XOR_CAS(32, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_xor_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64)
            {
                (void)order;
                return __sync_fetch_and_xor(dst, src);
            }
            #else
            {
                C89ATOMIC_FETCH_XOR_CAS(64, dst, src, order);
            }
            #endif
        }
    #elif defined(C89ATOMIC_LEGACY_GCC_ASM)
        /* Old GCC, or non-GCC compilers supporting GCC-style inlined assembly. The inlined assembly below uses Gas syntax. */
        #define C89ATOMIC_CMPXCHG_GCC_X86(instructionSizeSuffix, result, dst, expected, replacement) \
            __asm__ __volatile__(                             \
                "lock; cmpxchg"instructionSizeSuffix" %2, %1" \
                : "=a"(result),     /* %0 */                  \
                  "=m"(*dst)        /* %1 */                  \
                : "r"(replacement), /* %2 */                  \
                  "0"(expected),    /* %3 */                  \
                  "m"(*dst)         /* %4 */                  \
                : "cc", "memory")

        #define C89ATOMIC_XADD_GCC_X86(instructionSizeSuffix, result, dst, src) \
            __asm__ __volatile__(        \
                "lock; xadd"instructionSizeSuffix" %0, %1"      \
                : "=a"(result), /* %0 */ \
                  "=m"(*dst)    /* %1 */ \
                : "0"(src),     /* %2 */ \
                  "m"(*dst)     /* %3 */ \
                : "cc", "memory")


        /* compare_and_swap() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_compare_and_swap_8(volatile c89atomic_uint8* dst, c89atomic_uint8 expected, c89atomic_uint8 replacement)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint8 result;
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    C89ATOMIC_CMPXCHG_GCC_X86("b", result, dst, expected, replacement);
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_COMPARE_AND_SWAP_LOCK(8, dst, expected, replacement);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_compare_and_swap_16(volatile c89atomic_uint16* dst, c89atomic_uint16 expected, c89atomic_uint16 replacement)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint16 result;
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    C89ATOMIC_CMPXCHG_GCC_X86("w", result, dst, expected, replacement);
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_COMPARE_AND_SWAP_LOCK(16, dst, expected, replacement);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_compare_and_swap_32(volatile c89atomic_uint32* dst, c89atomic_uint32 expected, c89atomic_uint32 replacement)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint32 result;
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    C89ATOMIC_CMPXCHG_GCC_X86("l", result, dst, expected, replacement);
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_COMPARE_AND_SWAP_LOCK(32, dst, expected, replacement);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_compare_and_swap_64(volatile c89atomic_uint64* dst, c89atomic_uint64 expected, c89atomic_uint64 replacement)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint64 result;
                #if defined(C89ATOMIC_X86)
                {
                    /*
                    We can't use the standard CMPXCHG here because x86 does not support it with 64-bit values. We need to instead use CMPXCHG8B
                    which is a bit harder to use. The annoying part with this is the use of the -fPIC compiler switch which requires the EBX
                    register never be modified. The problem is that CMPXCHG8B requires us to write our replacement value to it. I'm resolving this
                    by just pushing and popping the EBX register manually.
                    */
                    c89atomic_uint32 resultEAX;
                    c89atomic_uint32 resultEDX;
                    __asm__ __volatile__(
                        "pushl %%ebx\n"
                        "movl  %4, %%ebx\n"
                        "lock  cmpxchg8b (%%edi)\n"
                        "popl  %%ebx\n"
                        : "=a"(resultEAX),
                          "=d"(resultEDX)
                        : "a"((c89atomic_uint32)(expected & 0xFFFFFFFF)),    /* EAX */
                          "d"((c89atomic_uint32)(expected >> 32)),           /* EDX */
                          "r"((c89atomic_uint32)(replacement & 0xFFFFFFFF)), /* %4  */
                          "c"((c89atomic_uint32)(replacement >> 32)),        /* ECX */
                          "D"(dst)                                           /* EDI */
                        : "memory", "cc");
                    result = ((c89atomic_uint64)resultEDX << 32) | resultEAX;
                }
                #elif defined(C89ATOMIC_X64)
                {
                    C89ATOMIC_CMPXCHG_GCC_X86("q", result, dst, expected, replacement);
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_COMPARE_AND_SWAP_LOCK(64, dst, expected, replacement);
            }
            #endif
        }


        /* Atomic loads can be implemented in terms of a compare-and-swap. Need to implement as functions to silence warnings about `order` being unused. */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_load_explicit_8(volatile const c89atomic_uint8* dst, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint8 result;
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    if (order == c89atomic_memory_order_relaxed) {
                        C89ATOMIC_LOAD_RELAXED_GCC_X86("b", result, dst);
                    } else if (order <= c89atomic_memory_order_release) {
                        C89ATOMIC_LOAD_RELEASE_GCC_X86("b", result, dst);
                    } else {
                        C89ATOMIC_LOAD_SEQ_CST_GCC_X86("b", result, dst);
                    }
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_LOAD_EXPLICIT_LOCK(8, dst, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_load_explicit_16(volatile const c89atomic_uint16* dst, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint16 result;
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    if (order == c89atomic_memory_order_relaxed) {
                        C89ATOMIC_LOAD_RELAXED_GCC_X86("w", result, dst);
                    } else if (order <= c89atomic_memory_order_release) {
                        C89ATOMIC_LOAD_RELEASE_GCC_X86("w", result, dst);
                    } else {
                        C89ATOMIC_LOAD_SEQ_CST_GCC_X86("w", result, dst);
                    }
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_LOAD_EXPLICIT_LOCK(16, dst, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_load_explicit_32(volatile const c89atomic_uint32* dst, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint32 result;
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    if (order == c89atomic_memory_order_relaxed) {
                        C89ATOMIC_LOAD_RELAXED_GCC_X86("l", result, dst);
                    } else if (order <= c89atomic_memory_order_release) {
                        C89ATOMIC_LOAD_RELEASE_GCC_X86("l", result, dst);
                    } else {
                        C89ATOMIC_LOAD_SEQ_CST_GCC_X86("l", result, dst);
                    }
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_LOAD_EXPLICIT_LOCK(32, dst, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_load_explicit_64(volatile const c89atomic_uint64* dst, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint64 result;
                #if defined(C89ATOMIC_X64)
                {
                    if (order == c89atomic_memory_order_relaxed) {
                        C89ATOMIC_LOAD_RELAXED_GCC_X86("q", result, dst);
                    } else if (order <= c89atomic_memory_order_release) {
                        C89ATOMIC_LOAD_RELEASE_GCC_X86("q", result, dst);
                    } else {
                        C89ATOMIC_LOAD_SEQ_CST_GCC_X86("q", result, dst);
                    }
                }
                #elif defined(C89ATOMIC_X86)
                {
                    (void)order;
                    return c89atomic_compare_and_swap_64((volatile c89atomic_uint64*)dst, 0, 0);
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_LOAD_EXPLICIT_LOCK(64, dst, order);
            }
            #endif
        }


        /* exchange() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_exchange_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint8 result;
                (void)order;
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    C89ATOMIC_XCHG_GCC_X86("b", result, dst, src);
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(8, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_exchange_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint16 result;
                (void)order;
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    C89ATOMIC_XCHG_GCC_X86("w", result, dst, src);
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(16, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_exchange_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint32 result;
                (void)order;
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    C89ATOMIC_XCHG_GCC_X86("l", result, dst, src);
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(32, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_exchange_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                c89atomic_uint64 result;
                (void)order;
                #if defined(C89ATOMIC_X86)
                {
                    C89ATOMIC_EXCHANGE_EXPLICIT_CAS(64, dst, src, order);
                }
                #elif defined(C89ATOMIC_X64)
                {
                    C89ATOMIC_XCHG_GCC_X86("q", result, dst, src);
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
                return result;
            }
            #else
            {
                C89ATOMIC_EXCHANGE_EXPLICIT_LOCK(64, dst, src, order);
            }
            #endif
        }


        /* store() */
        static C89ATOMIC_INLINE void c89atomic_store_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    if (order == c89atomic_memory_order_relaxed) {
                        __asm__ __volatile__ (
                            "movb %1, %0"
                            : "=m"(*dst)    /* %0 */
                            : "r"(src)      /* %1 */
                        );
                    } else {
                        __asm__ __volatile__ (
                            "xchgb %1, %0"
                            : "=m"(*dst)    /* %0 */
                            : "r"(src)      /* %1 */
                            : "memory"
                        );
                    }
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
            }
            #else
            {
                C89ATOMIC_STORE_EXPLICIT_LOCK(8, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE void c89atomic_store_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    if (order == c89atomic_memory_order_relaxed) {
                        __asm__ __volatile__ (
                            "movw %1, %0"
                            : "=m"(*dst)    /* %0 */
                            : "r"(src)      /* %1 */
                        );
                    } else {
                        __asm__ __volatile__ (
                            "xchgw %1, %0"
                            : "=m"(*dst)    /* %0 */
                            : "r"(src)      /* %1 */
                            : "memory"
                        );
                    }
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
            }
            #else
            {
                C89ATOMIC_STORE_EXPLICIT_LOCK(16, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE void c89atomic_store_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    if (order == c89atomic_memory_order_relaxed) {
                        __asm__ __volatile__ (
                            "movl %1, %0"
                            : "=m"(*dst)    /* %0 */
                            : "r"(src)      /* %1 */
                        );
                    } else {
                        __asm__ __volatile__ (
                            "xchgl %1, %0"
                            : "=m"(*dst)    /* %0 */
                            : "r"(src)      /* %1 */
                            : "memory"
                        );
                    }
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
            }
            #else
            {
                C89ATOMIC_STORE_EXPLICIT_LOCK(32, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE void c89atomic_store_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                #if defined(C89ATOMIC_X64)
                {
                    if (order == c89atomic_memory_order_relaxed) {
                        __asm__ __volatile__ (
                            "movq %1, %0"
                            : "=m"(*dst)    /* %0 */
                            : "r"(src)      /* %1 */
                        );
                    } else {
                        __asm__ __volatile__ (
                            "xchgq %1, %0"
                            : "=m"(*dst)    /* %0 */
                            : "r"(src)      /* %1 */
                            : "memory"
                        );
                    }
                }
                #else
                {
                    C89ATOMIC_STORE_EXPLICIT_CAS(64, dst, src, order);
                }
                #endif
            }
            #else
            {
                C89ATOMIC_STORE_EXPLICIT_LOCK(64, dst, src, order);
            }
            #endif
        }


        /* fetch_add() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_add_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_8) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    c89atomic_uint8 result;

                    (void)order;
                    C89ATOMIC_XADD_GCC_X86("b", result, dst, src);

                    return result;
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(8, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_add_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_16) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    c89atomic_uint16 result;

                    (void)order;
                    C89ATOMIC_XADD_GCC_X86("w", result, dst, src);

                    return result;
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(16, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_add_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_32) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
                {
                    c89atomic_uint32 result;

                    (void)order;
                    C89ATOMIC_XADD_GCC_X86("l", result, dst, src);

                    return result;
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(32, dst, src, order);
            }
            #endif
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_add_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            #if defined(C89ATOMIC_IS_LOCK_FREE_64) && (defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64))
            {
                #if defined(C89ATOMIC_X86)
                {
                    C89ATOMIC_FETCH_ADD_CAS(64, dst, src, order);
                }
                #elif defined(C89ATOMIC_X64)
                {
                    c89atomic_uint64 result;

                    C89ATOMIC_XADD_GCC_X86("q", result, dst, src);

                    (void)order;
                    return result;
                }
                #else
                {
                    #error Unsupported architecture.
                }
                #endif
            }
            #else
            {
                C89ATOMIC_FETCH_ADD_LOCK(64, dst, src, order);
            }
            #endif
        }


        /* fetch_sub() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_sub_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            return c89atomic_fetch_add_explicit_8(dst, (c89atomic_uint8)(-(c89atomic_int8)src), order);
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_sub_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            return c89atomic_fetch_add_explicit_16(dst, (c89atomic_uint16)(-(c89atomic_int16)src), order);
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_sub_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            return c89atomic_fetch_add_explicit_32(dst, (c89atomic_uint32)(-(c89atomic_int32)src), order);
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_sub_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            return c89atomic_fetch_add_explicit_64(dst, (c89atomic_uint64)(-(c89atomic_int64)src), order);
        }


        /* fetch_and() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_and_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_AND_CAS(8, dst, src, order);
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_and_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_AND_CAS(16, dst, src, order);
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_and_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_AND_CAS(32, dst, src, order);
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_and_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_AND_CAS(64, dst, src, order);
        }


        /* fetch_or() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_or_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_OR_CAS(8, dst, src, order);
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_or_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_OR_CAS(16, dst, src, order);
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_or_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_OR_CAS(32, dst, src, order);
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_or_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_OR_CAS(64, dst, src, order);
        }


        /* fetch_xor() */
        static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_fetch_xor_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_XOR_CAS(8, dst, src, order);
        }

        static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_fetch_xor_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_XOR_CAS(16, dst, src, order);
        }

        static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_fetch_xor_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_XOR_CAS(32, dst, src, order);
        }

        static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_fetch_xor_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
        {
            C89ATOMIC_FETCH_XOR_CAS(64, dst, src, order);
        }
    #else
        #error Unsupported compiler.
    #endif
#endif



/*
Everything below this point is stuff that is implemented in terms of the functions defined above.
*/

/* compare_exchange() */
#if !defined(C89ATOMIC_HAS_NATIVE_COMPARE_EXCHANGE)
    static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_strong_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8* expected, c89atomic_uint8 replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
    {
        c89atomic_uint8 result;

        (void)successOrder;
        (void)failureOrder;

        result = c89atomic_compare_and_swap_8(dst, *expected, replacement);
        if (result == *expected) {
            return 1;
        } else {
            *expected = result;
            return 0;
        }
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_strong_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16* expected, c89atomic_uint16 replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
    {
        c89atomic_uint16 result;

        (void)successOrder;
        (void)failureOrder;

        result = c89atomic_compare_and_swap_16(dst, *expected, replacement);
        if (result == *expected) {
            return 1;
        } else {
            *expected = result;
            return 0;
        }
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_strong_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32* expected, c89atomic_uint32 replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
    {
        c89atomic_uint32 result;

        (void)successOrder;
        (void)failureOrder;

        result = c89atomic_compare_and_swap_32(dst, *expected, replacement);
        if (result == *expected) {
            return 1;
        } else {
            *expected = result;
            return 0;
        }
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_strong_explicit_64(volatile c89atomic_uint64* dst, volatile c89atomic_uint64* expected, c89atomic_uint64 replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
    {
        c89atomic_uint64 result;

        (void)successOrder;
        (void)failureOrder;

        result = c89atomic_compare_and_swap_64(dst, *expected, replacement);
        if (result == *expected) {
            return 1;
        } else {
            *expected = result;
            return 0;
        }
    }

    #define c89atomic_compare_exchange_weak_explicit_8( dst, expected, replacement, successOrder, failureOrder) c89atomic_compare_exchange_strong_explicit_8 (dst, expected, replacement, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_16(dst, expected, replacement, successOrder, failureOrder) c89atomic_compare_exchange_strong_explicit_16(dst, expected, replacement, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_32(dst, expected, replacement, successOrder, failureOrder) c89atomic_compare_exchange_strong_explicit_32(dst, expected, replacement, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_64(dst, expected, replacement, successOrder, failureOrder) c89atomic_compare_exchange_strong_explicit_64(dst, expected, replacement, successOrder, failureOrder)
#endif  /* C89ATOMIC_HAS_NATIVE_COMPARE_EXCHANGE */


/*
Pointer versions of relevant operations. Note that some functions cannot be implemented as #defines because for some reason, some compilers
complain with a warning if you don't use the return value. I'm not fully sure why this happens, but to work around this, those particular
functions are just implemented as inlined functions.
*/
#if defined(C89ATOMIC_64BIT)
    static C89ATOMIC_INLINE c89atomic_bool c89atomic_is_lock_free_ptr(volatile void** ptr)
    {
        return c89atomic_is_lock_free_64((volatile c89atomic_uint64*)ptr);
    }

    static C89ATOMIC_INLINE void* c89atomic_load_explicit_ptr(volatile void** ptr, c89atomic_memory_order order)
    {
        return (void*)c89atomic_load_explicit_64((volatile c89atomic_uint64*)ptr, order);
    }

    static C89ATOMIC_INLINE void c89atomic_store_explicit_ptr(volatile void** dst, void* src, c89atomic_memory_order order)
    {
        c89atomic_store_explicit_64((volatile c89atomic_uint64*)dst, (c89atomic_uint64)src, order);
    }

    static C89ATOMIC_INLINE void* c89atomic_exchange_explicit_ptr(volatile void** dst, void* src, c89atomic_memory_order order)
    {
        return (void*)c89atomic_exchange_explicit_64((volatile c89atomic_uint64*)dst, (c89atomic_uint64)src, order);
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_strong_explicit_ptr(volatile void** dst, void** expected, void* replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
    {
        return c89atomic_compare_exchange_strong_explicit_64((volatile c89atomic_uint64*)dst, (c89atomic_uint64*)expected, (c89atomic_uint64)replacement, successOrder, failureOrder);
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_weak_explicit_ptr(volatile void** dst, void** expected, void* replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
    {
        return c89atomic_compare_exchange_weak_explicit_64((volatile c89atomic_uint64*)dst, (c89atomic_uint64*)expected, (c89atomic_uint64)replacement, successOrder, failureOrder);
    }

    static C89ATOMIC_INLINE void* c89atomic_compare_and_swap_ptr(volatile void** dst, void* expected, void* replacement)
    {
        return (void*)c89atomic_compare_and_swap_64((volatile c89atomic_uint64*)dst, (c89atomic_uint64)expected, (c89atomic_uint64)replacement);
    }
#elif defined(C89ATOMIC_32BIT)
    static C89ATOMIC_INLINE c89atomic_bool c89atomic_is_lock_free_ptr(volatile void** ptr)
    {
        return c89atomic_is_lock_free_32((volatile c89atomic_uint32*)ptr);
    }

    static C89ATOMIC_INLINE void* c89atomic_load_explicit_ptr(volatile void** ptr, c89atomic_memory_order order)
    {
        return (void*)c89atomic_load_explicit_32((volatile c89atomic_uint32*)ptr, order);
    }

    static C89ATOMIC_INLINE void c89atomic_store_explicit_ptr(volatile void** dst, void* src, c89atomic_memory_order order)
    {
        c89atomic_store_explicit_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32)src, order);
    }

    static C89ATOMIC_INLINE void* c89atomic_exchange_explicit_ptr(volatile void** dst, void* src, c89atomic_memory_order order)
    {
        return (void*)c89atomic_exchange_explicit_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32)src, order);
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_strong_explicit_ptr(volatile void** dst, void** expected, void* replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
    {
        return c89atomic_compare_exchange_strong_explicit_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32*)expected, (c89atomic_uint32)replacement, successOrder, failureOrder);
    }

    static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_weak_explicit_ptr(volatile void** dst, void** expected, void* replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
    {
        return c89atomic_compare_exchange_weak_explicit_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32*)expected, (c89atomic_uint32)replacement, successOrder, failureOrder);
    }

    static C89ATOMIC_INLINE void* c89atomic_compare_and_swap_ptr(volatile void** dst, void* expected, void* replacement)
    {
        return (void*)c89atomic_compare_and_swap_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32)expected, (c89atomic_uint32)replacement);
    }
#else
    #error Unsupported architecture.
#endif


/* Implicit Pointer. */
#define c89atomic_store_ptr(dst, src)                                       c89atomic_store_explicit_ptr((volatile void**)dst, (void*)src, c89atomic_memory_order_seq_cst)
#define c89atomic_load_ptr(ptr)                                             c89atomic_load_explicit_ptr((volatile void**)ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_ptr(dst, src)                                    c89atomic_exchange_explicit_ptr((volatile void**)dst, (void*)src, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_ptr(dst, expected, replacement)   c89atomic_compare_exchange_strong_explicit_ptr((volatile void**)dst, (void**)expected, (void*)replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_ptr(dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_ptr((volatile void**)dst, (void**)expected, (void*)replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)


/* Implicit Unsigned Integer. */
#define c89atomic_store_8( dst, src)                                    c89atomic_store_explicit_8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_16(dst, src)                                    c89atomic_store_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_32(dst, src)                                    c89atomic_store_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_64(dst, src)                                    c89atomic_store_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_load_8( ptr)                                          c89atomic_load_explicit_8( ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_16(ptr)                                          c89atomic_load_explicit_16(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_32(ptr)                                          c89atomic_load_explicit_32(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_64(ptr)                                          c89atomic_load_explicit_64(ptr, c89atomic_memory_order_seq_cst)

#define c89atomic_exchange_8( dst, src)                                 c89atomic_exchange_explicit_8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_16(dst, src)                                 c89atomic_exchange_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_32(dst, src)                                 c89atomic_exchange_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_64(dst, src)                                 c89atomic_exchange_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_compare_exchange_strong_8( dst, expected, replacement)    c89atomic_compare_exchange_strong_explicit_8( dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_16(dst, expected, replacement)    c89atomic_compare_exchange_strong_explicit_16(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_32(dst, expected, replacement)    c89atomic_compare_exchange_strong_explicit_32(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_64(dst, expected, replacement)    c89atomic_compare_exchange_strong_explicit_64(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)

#define c89atomic_compare_exchange_weak_8(  dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_8( dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_16( dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_16(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_32( dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_32(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_64( dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_64(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_add_8( dst, src)                                c89atomic_fetch_add_explicit_8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_16(dst, src)                                c89atomic_fetch_add_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_32(dst, src)                                c89atomic_fetch_add_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_64(dst, src)                                c89atomic_fetch_add_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_sub_8( dst, src)                                c89atomic_fetch_sub_explicit_8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_16(dst, src)                                c89atomic_fetch_sub_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_32(dst, src)                                c89atomic_fetch_sub_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_64(dst, src)                                c89atomic_fetch_sub_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_or_8( dst, src)                                 c89atomic_fetch_or_explicit_8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_16(dst, src)                                 c89atomic_fetch_or_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_32(dst, src)                                 c89atomic_fetch_or_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_64(dst, src)                                 c89atomic_fetch_or_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_xor_8( dst, src)                                c89atomic_fetch_xor_explicit_8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_16(dst, src)                                c89atomic_fetch_xor_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_32(dst, src)                                c89atomic_fetch_xor_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_64(dst, src)                                c89atomic_fetch_xor_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_and_8( dst, src)                                c89atomic_fetch_and_explicit_8 (dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_16(dst, src)                                c89atomic_fetch_and_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_32(dst, src)                                c89atomic_fetch_and_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_64(dst, src)                                c89atomic_fetch_and_explicit_64(dst, src, c89atomic_memory_order_seq_cst)


/* Explicit Signed Integer. */
#define c89atomic_store_explicit_i8( dst, src, order)                   c89atomic_store_explicit_8( (c89atomic_uint8* )dst, (c89atomic_uint8 )src, order)
#define c89atomic_store_explicit_i16(dst, src, order)                   c89atomic_store_explicit_16((c89atomic_uint16*)dst, (c89atomic_uint16)src, order)
#define c89atomic_store_explicit_i32(dst, src, order)                   c89atomic_store_explicit_32((c89atomic_uint32*)dst, (c89atomic_uint32)src, order)
#define c89atomic_store_explicit_i64(dst, src, order)                   c89atomic_store_explicit_64((c89atomic_uint64*)dst, (c89atomic_uint64)src, order)

#define c89atomic_load_explicit_i8( ptr, order)                         (c89atomic_int8 )c89atomic_load_explicit_8( (c89atomic_uint8* )ptr, order)
#define c89atomic_load_explicit_i16(ptr, order)                         (c89atomic_int16)c89atomic_load_explicit_16((c89atomic_uint16*)ptr, order)
#define c89atomic_load_explicit_i32(ptr, order)                         (c89atomic_int32)c89atomic_load_explicit_32((c89atomic_uint32*)ptr, order)
#define c89atomic_load_explicit_i64(ptr, order)                         (c89atomic_int64)c89atomic_load_explicit_64((c89atomic_uint64*)ptr, order)

#define c89atomic_exchange_explicit_i8( dst, src, order)                (c89atomic_int8 )c89atomic_exchange_explicit_8 ((c89atomic_uint8* )dst, (c89atomic_uint8 )src, order)
#define c89atomic_exchange_explicit_i16(dst, src, order)                (c89atomic_int16)c89atomic_exchange_explicit_16((c89atomic_uint16*)dst, (c89atomic_uint16)src, order)
#define c89atomic_exchange_explicit_i32(dst, src, order)                (c89atomic_int32)c89atomic_exchange_explicit_32((c89atomic_uint32*)dst, (c89atomic_uint32)src, order)
#define c89atomic_exchange_explicit_i64(dst, src, order)                (c89atomic_int64)c89atomic_exchange_explicit_64((c89atomic_uint64*)dst, (c89atomic_uint64)src, order)

#define c89atomic_compare_exchange_strong_explicit_i8( dst, expected, replacement, successOrder, failureOrder)  c89atomic_compare_exchange_strong_explicit_8( (c89atomic_uint8* )dst, (c89atomic_uint8* )expected, (c89atomic_uint8 )replacement, successOrder, failureOrder)
#define c89atomic_compare_exchange_strong_explicit_i16(dst, expected, replacement, successOrder, failureOrder)  c89atomic_compare_exchange_strong_explicit_16((c89atomic_uint16*)dst, (c89atomic_uint16*)expected, (c89atomic_uint16)replacement, successOrder, failureOrder)
#define c89atomic_compare_exchange_strong_explicit_i32(dst, expected, replacement, successOrder, failureOrder)  c89atomic_compare_exchange_strong_explicit_32((c89atomic_uint32*)dst, (c89atomic_uint32*)expected, (c89atomic_uint32)replacement, successOrder, failureOrder)
#define c89atomic_compare_exchange_strong_explicit_i64(dst, expected, replacement, successOrder, failureOrder)  c89atomic_compare_exchange_strong_explicit_64((c89atomic_uint64*)dst, (c89atomic_uint64*)expected, (c89atomic_uint64)replacement, successOrder, failureOrder)

#define c89atomic_compare_exchange_weak_explicit_i8( dst, expected, replacement, successOrder, failureOrder)    c89atomic_compare_exchange_weak_explicit_8( (c89atomic_uint8* )dst, (c89atomic_uint8* )expected, (c89atomic_uint8 )replacement, successOrder, failureOrder)
#define c89atomic_compare_exchange_weak_explicit_i16(dst, expected, replacement, successOrder, failureOrder)    c89atomic_compare_exchange_weak_explicit_16((c89atomic_uint16*)dst, (c89atomic_uint16*)expected, (c89atomic_uint16)replacement, successOrder, failureOrder)
#define c89atomic_compare_exchange_weak_explicit_i32(dst, expected, replacement, successOrder, failureOrder)    c89atomic_compare_exchange_weak_explicit_32((c89atomic_uint32*)dst, (c89atomic_uint32*)expected, (c89atomic_uint32)replacement, successOrder, failureOrder)
#define c89atomic_compare_exchange_weak_explicit_i64(dst, expected, replacement, successOrder, failureOrder)    c89atomic_compare_exchange_weak_explicit_64((c89atomic_uint64*)dst, (c89atomic_uint64*)expected, (c89atomic_uint64)replacement, successOrder, failureOrder)

#define c89atomic_fetch_add_explicit_i8( dst, src, order)               (c89atomic_int8 )c89atomic_fetch_add_explicit_8( (c89atomic_uint8* )dst, (c89atomic_uint8 )src, order)
#define c89atomic_fetch_add_explicit_i16(dst, src, order)               (c89atomic_int16)c89atomic_fetch_add_explicit_16((c89atomic_uint16*)dst, (c89atomic_uint16)src, order)
#define c89atomic_fetch_add_explicit_i32(dst, src, order)               (c89atomic_int32)c89atomic_fetch_add_explicit_32((c89atomic_uint32*)dst, (c89atomic_uint32)src, order)
#define c89atomic_fetch_add_explicit_i64(dst, src, order)               (c89atomic_int64)c89atomic_fetch_add_explicit_64((c89atomic_uint64*)dst, (c89atomic_uint64)src, order)

#define c89atomic_fetch_sub_explicit_i8( dst, src, order)               (c89atomic_int8 )c89atomic_fetch_sub_explicit_8( (c89atomic_uint8* )dst, (c89atomic_uint8 )src, order)
#define c89atomic_fetch_sub_explicit_i16(dst, src, order)               (c89atomic_int16)c89atomic_fetch_sub_explicit_16((c89atomic_uint16*)dst, (c89atomic_uint16)src, order)
#define c89atomic_fetch_sub_explicit_i32(dst, src, order)               (c89atomic_int32)c89atomic_fetch_sub_explicit_32((c89atomic_uint32*)dst, (c89atomic_uint32)src, order)
#define c89atomic_fetch_sub_explicit_i64(dst, src, order)               (c89atomic_int64)c89atomic_fetch_sub_explicit_64((c89atomic_uint64*)dst, (c89atomic_uint64)src, order)

#define c89atomic_fetch_or_explicit_i8( dst, src, order)                (c89atomic_int8 )c89atomic_fetch_or_explicit_8( (c89atomic_uint8* )dst, (c89atomic_uint8 )src, order)
#define c89atomic_fetch_or_explicit_i16(dst, src, order)                (c89atomic_int16)c89atomic_fetch_or_explicit_16((c89atomic_uint16*)dst, (c89atomic_uint16)src, order)
#define c89atomic_fetch_or_explicit_i32(dst, src, order)                (c89atomic_int32)c89atomic_fetch_or_explicit_32((c89atomic_uint32*)dst, (c89atomic_uint32)src, order)
#define c89atomic_fetch_or_explicit_i64(dst, src, order)                (c89atomic_int64)c89atomic_fetch_or_explicit_64((c89atomic_uint64*)dst, (c89atomic_uint64)src, order)

#define c89atomic_fetch_xor_explicit_i8( dst, src, order)               (c89atomic_int8 )c89atomic_fetch_xor_explicit_8( (c89atomic_uint8* )dst, (c89atomic_uint8 )src, order)
#define c89atomic_fetch_xor_explicit_i16(dst, src, order)               (c89atomic_int16)c89atomic_fetch_xor_explicit_16((c89atomic_uint16*)dst, (c89atomic_uint16)src, order)
#define c89atomic_fetch_xor_explicit_i32(dst, src, order)               (c89atomic_int32)c89atomic_fetch_xor_explicit_32((c89atomic_uint32*)dst, (c89atomic_uint32)src, order)
#define c89atomic_fetch_xor_explicit_i64(dst, src, order)               (c89atomic_int64)c89atomic_fetch_xor_explicit_64((c89atomic_uint64*)dst, (c89atomic_uint64)src, order)

#define c89atomic_fetch_and_explicit_i8( dst, src, order)               (c89atomic_int8 )c89atomic_fetch_and_explicit_8( (c89atomic_uint8* )dst, (c89atomic_uint8 )src, order)
#define c89atomic_fetch_and_explicit_i16(dst, src, order)               (c89atomic_int16)c89atomic_fetch_and_explicit_16((c89atomic_uint16*)dst, (c89atomic_uint16)src, order)
#define c89atomic_fetch_and_explicit_i32(dst, src, order)               (c89atomic_int32)c89atomic_fetch_and_explicit_32((c89atomic_uint32*)dst, (c89atomic_uint32)src, order)
#define c89atomic_fetch_and_explicit_i64(dst, src, order)               (c89atomic_int64)c89atomic_fetch_and_explicit_64((c89atomic_uint64*)dst, (c89atomic_uint64)src, order)


/* Implicit Signed Integer. */
#define c89atomic_store_i8( dst, src)                                   c89atomic_store_explicit_i8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_i16(dst, src)                                   c89atomic_store_explicit_i16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_i32(dst, src)                                   c89atomic_store_explicit_i32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_i64(dst, src)                                   c89atomic_store_explicit_i64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_load_i8( ptr)                                         c89atomic_load_explicit_i8( ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_i16(ptr)                                         c89atomic_load_explicit_i16(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_i32(ptr)                                         c89atomic_load_explicit_i32(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_i64(ptr)                                         c89atomic_load_explicit_i64(ptr, c89atomic_memory_order_seq_cst)

#define c89atomic_exchange_i8( dst, src)                                c89atomic_exchange_explicit_i8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_i16(dst, src)                                c89atomic_exchange_explicit_i16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_i32(dst, src)                                c89atomic_exchange_explicit_i32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_i64(dst, src)                                c89atomic_exchange_explicit_i64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_compare_exchange_strong_i8( dst, expected, replacement)   c89atomic_compare_exchange_strong_explicit_i8( dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_i16(dst, expected, replacement)   c89atomic_compare_exchange_strong_explicit_i16(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_i32(dst, expected, replacement)   c89atomic_compare_exchange_strong_explicit_i32(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_i64(dst, expected, replacement)   c89atomic_compare_exchange_strong_explicit_i64(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)

#define c89atomic_compare_exchange_weak_i8( dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_i8( dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_i16(dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_i16(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_i32(dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_i32(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_i64(dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_i64(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_add_i8( dst, src)                               c89atomic_fetch_add_explicit_i8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_i16(dst, src)                               c89atomic_fetch_add_explicit_i16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_i32(dst, src)                               c89atomic_fetch_add_explicit_i32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_i64(dst, src)                               c89atomic_fetch_add_explicit_i64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_sub_i8( dst, src)                               c89atomic_fetch_sub_explicit_i8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_i16(dst, src)                               c89atomic_fetch_sub_explicit_i16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_i32(dst, src)                               c89atomic_fetch_sub_explicit_i32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_i64(dst, src)                               c89atomic_fetch_sub_explicit_i64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_or_i8( dst, src)                                c89atomic_fetch_or_explicit_i8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_i16(dst, src)                                c89atomic_fetch_or_explicit_i16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_i32(dst, src)                                c89atomic_fetch_or_explicit_i32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_i64(dst, src)                                c89atomic_fetch_or_explicit_i64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_xor_i8( dst, src)                               c89atomic_fetch_xor_explicit_i8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_i16(dst, src)                               c89atomic_fetch_xor_explicit_i16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_i32(dst, src)                               c89atomic_fetch_xor_explicit_i32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_i64(dst, src)                               c89atomic_fetch_xor_explicit_i64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_and_i8( dst, src)                               c89atomic_fetch_and_explicit_i8( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_i16(dst, src)                               c89atomic_fetch_and_explicit_i16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_i32(dst, src)                               c89atomic_fetch_and_explicit_i32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_i64(dst, src)                               c89atomic_fetch_and_explicit_i64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_compare_and_swap_i8( dst, expected, dedsired)         (c89atomic_int8 )c89atomic_compare_and_swap_8( (c89atomic_uint8* )dst, (c89atomic_uint8 )expected, (c89atomic_uint8 )dedsired)
#define c89atomic_compare_and_swap_i16(dst, expected, dedsired)         (c89atomic_int16)c89atomic_compare_and_swap_16((c89atomic_uint16*)dst, (c89atomic_uint16)expected, (c89atomic_uint16)dedsired)
#define c89atomic_compare_and_swap_i32(dst, expected, dedsired)         (c89atomic_int32)c89atomic_compare_and_swap_32((c89atomic_uint32*)dst, (c89atomic_uint32)expected, (c89atomic_uint32)dedsired)
#define c89atomic_compare_and_swap_i64(dst, expected, dedsired)         (c89atomic_int64)c89atomic_compare_and_swap_64((c89atomic_uint64*)dst, (c89atomic_uint64)expected, (c89atomic_uint64)dedsired)


/* Floating Point Explicit. */
typedef union
{
    c89atomic_uint32 i;
    float f;
} c89atomic_if32;

typedef union
{
    c89atomic_uint64 i;
    double f;
} c89atomic_if64;

#define c89atomic_clear_explicit_f32(ptr, order)                        c89atomic_clear_explicit_32((c89atomic_uint32*)ptr, order)
#define c89atomic_clear_explicit_f64(ptr, order)                        c89atomic_clear_explicit_64((c89atomic_uint64*)ptr, order)

static C89ATOMIC_INLINE void c89atomic_store_explicit_f32(volatile float* dst, float src, c89atomic_memory_order order)
{
    c89atomic_if32 x;
    x.f = src;
    c89atomic_store_explicit_32((volatile c89atomic_uint32*)dst, x.i, order);
}

static C89ATOMIC_INLINE void c89atomic_store_explicit_f64(volatile double* dst, double src, c89atomic_memory_order order)
{
    c89atomic_if64 x;
    x.f = src;
    c89atomic_store_explicit_64((volatile c89atomic_uint64*)dst, x.i, order);
}


static C89ATOMIC_INLINE float c89atomic_load_explicit_f32(volatile const float* ptr, c89atomic_memory_order order)
{
    c89atomic_if32 r;
    r.i = c89atomic_load_explicit_32((volatile const c89atomic_uint32*)ptr, order);
    return r.f;
}

static C89ATOMIC_INLINE double c89atomic_load_explicit_f64(volatile const double* ptr, c89atomic_memory_order order)
{
    c89atomic_if64 r;
    r.i = c89atomic_load_explicit_64((volatile const c89atomic_uint64*)ptr, order);
    return r.f;
}


static C89ATOMIC_INLINE float c89atomic_exchange_explicit_f32(volatile float* dst, float src, c89atomic_memory_order order)
{
    c89atomic_if32 r;
    c89atomic_if32 x;
    x.f = src;
    r.i = c89atomic_exchange_explicit_32((volatile c89atomic_uint32*)dst, x.i, order);
    return r.f;
}

static C89ATOMIC_INLINE double c89atomic_exchange_explicit_f64(volatile double* dst, double src, c89atomic_memory_order order)
{
    c89atomic_if64 r;
    c89atomic_if64 x;
    x.f = src;
    r.i = c89atomic_exchange_explicit_64((volatile c89atomic_uint64*)dst, x.i, order);
    return r.f;
}


static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_strong_explicit_f32(volatile float* dst, float* expected, float replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
{
    c89atomic_if32 d;
    d.f = replacement;
    return c89atomic_compare_exchange_strong_explicit_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32*)expected, d.i, successOrder, failureOrder);
}

static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_strong_explicit_f64(volatile double* dst, double* expected, double replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
{
    c89atomic_if64 d;
    d.f = replacement;
    return c89atomic_compare_exchange_strong_explicit_64((volatile c89atomic_uint64*)dst, (c89atomic_uint64*)expected, d.i, successOrder, failureOrder);
}


static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_weak_explicit_f32(volatile float* dst, float* expected, float replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
{
    c89atomic_if32 d;
    d.f = replacement;
    return c89atomic_compare_exchange_weak_explicit_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32*)expected, d.i, successOrder, failureOrder);
}

static C89ATOMIC_INLINE c89atomic_bool c89atomic_compare_exchange_weak_explicit_f64(volatile double* dst, double* expected, double replacement, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
{
    c89atomic_if64 d;
    d.f = replacement;
    return c89atomic_compare_exchange_weak_explicit_64((volatile c89atomic_uint64*)dst, (c89atomic_uint64*)expected, d.i, successOrder, failureOrder);
}


static C89ATOMIC_INLINE float c89atomic_fetch_add_explicit_f32(volatile float* dst, float src, c89atomic_memory_order order)
{
    c89atomic_if32 r;
    c89atomic_if32 x;
    x.f = src;
    r.i = c89atomic_fetch_add_explicit_32((volatile c89atomic_uint32*)dst, x.i, order);
    return r.f;
}

static C89ATOMIC_INLINE double c89atomic_fetch_add_explicit_f64(volatile double* dst, double src, c89atomic_memory_order order)
{
    c89atomic_if64 r;
    c89atomic_if64 x;
    x.f = src;
    r.i = c89atomic_fetch_add_explicit_64((volatile c89atomic_uint64*)dst, x.i, order);
    return r.f;
}


static C89ATOMIC_INLINE float c89atomic_fetch_sub_explicit_f32(volatile float* dst, float src, c89atomic_memory_order order)
{
    c89atomic_if32 r;
    c89atomic_if32 x;
    x.f = src;
    r.i = c89atomic_fetch_sub_explicit_32((volatile c89atomic_uint32*)dst, x.i, order);
    return r.f;
}

static C89ATOMIC_INLINE double c89atomic_fetch_sub_explicit_f64(volatile double* dst, double src, c89atomic_memory_order order)
{
    c89atomic_if64 r;
    c89atomic_if64 x;
    x.f = src;
    r.i = c89atomic_fetch_sub_explicit_64((volatile c89atomic_uint64*)dst, x.i, order);
    return r.f;
}


static C89ATOMIC_INLINE float c89atomic_fetch_or_explicit_f32(volatile float* dst, float src, c89atomic_memory_order order)
{
    c89atomic_if32 r;
    c89atomic_if32 x;
    x.f = src;
    r.i = c89atomic_fetch_or_explicit_32((volatile c89atomic_uint32*)dst, x.i, order);
    return r.f;
}

static C89ATOMIC_INLINE double c89atomic_fetch_or_explicit_f64(volatile double* dst, double src, c89atomic_memory_order order)
{
    c89atomic_if64 r;
    c89atomic_if64 x;
    x.f = src;
    r.i = c89atomic_fetch_or_explicit_64((volatile c89atomic_uint64*)dst, x.i, order);
    return r.f;
}


static C89ATOMIC_INLINE float c89atomic_fetch_xor_explicit_f32(volatile float* dst, float src, c89atomic_memory_order order)
{
    c89atomic_if32 r;
    c89atomic_if32 x;
    x.f = src;
    r.i = c89atomic_fetch_xor_explicit_32((volatile c89atomic_uint32*)dst, x.i, order);
    return r.f;
}

static C89ATOMIC_INLINE double c89atomic_fetch_xor_explicit_f64(volatile double* dst, double src, c89atomic_memory_order order)
{
    c89atomic_if64 r;
    c89atomic_if64 x;
    x.f = src;
    r.i = c89atomic_fetch_xor_explicit_64((volatile c89atomic_uint64*)dst, x.i, order);
    return r.f;
}


static C89ATOMIC_INLINE float c89atomic_fetch_and_explicit_f32(volatile float* dst, float src, c89atomic_memory_order order)
{
    c89atomic_if32 r;
    c89atomic_if32 x;
    x.f = src;
    r.i = c89atomic_fetch_and_explicit_32((volatile c89atomic_uint32*)dst, x.i, order);
    return r.f;
}

static C89ATOMIC_INLINE double c89atomic_fetch_and_explicit_f64(volatile double* dst, double src, c89atomic_memory_order order)
{
    c89atomic_if64 r;
    c89atomic_if64 x;
    x.f = src;
    r.i = c89atomic_fetch_and_explicit_64((volatile c89atomic_uint64*)dst, x.i, order);
    return r.f;
}



/* Float Point Implicit */
#define c89atomic_clear_f32(ptr)                                        (float )c89atomic_clear_explicit_f32(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_clear_f64(ptr)                                        (double)c89atomic_clear_explicit_f64(ptr, c89atomic_memory_order_seq_cst)

#define c89atomic_store_f32(dst, src)                                   c89atomic_store_explicit_f32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_f64(dst, src)                                   c89atomic_store_explicit_f64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_load_f32(ptr)                                         (float )c89atomic_load_explicit_f32(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_f64(ptr)                                         (double)c89atomic_load_explicit_f64(ptr, c89atomic_memory_order_seq_cst)

#define c89atomic_exchange_f32(dst, src)                                (float )c89atomic_exchange_explicit_f32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_f64(dst, src)                                (double)c89atomic_exchange_explicit_f64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_compare_exchange_strong_f32(dst, expected, replacement)   c89atomic_compare_exchange_strong_explicit_f32(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_f64(dst, expected, replacement)   c89atomic_compare_exchange_strong_explicit_f64(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)

#define c89atomic_compare_exchange_weak_f32(dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_f32(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_f64(dst, expected, replacement)     c89atomic_compare_exchange_weak_explicit_f64(dst, expected, replacement, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_add_f32(dst, src)                               c89atomic_fetch_add_explicit_f32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_f64(dst, src)                               c89atomic_fetch_add_explicit_f64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_sub_f32(dst, src)                               c89atomic_fetch_sub_explicit_f32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_f64(dst, src)                               c89atomic_fetch_sub_explicit_f64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_or_f32(dst, src)                                c89atomic_fetch_or_explicit_f32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_f64(dst, src)                                c89atomic_fetch_or_explicit_f64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_xor_f32(dst, src)                               c89atomic_fetch_xor_explicit_f32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_f64(dst, src)                               c89atomic_fetch_xor_explicit_f64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_and_f32(dst, src)                               c89atomic_fetch_and_explicit_f32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_f64(dst, src)                               c89atomic_fetch_and_explicit_f64(dst, src, c89atomic_memory_order_seq_cst)

static C89ATOMIC_INLINE float c89atomic_compare_and_swap_f32(volatile float* dst, float expected, float replacement)
{
    c89atomic_if32 r;
    c89atomic_if32 e, d;
    e.f = expected;
    d.f = replacement;
    r.i = c89atomic_compare_and_swap_32((volatile c89atomic_uint32*)dst, e.i, d.i);
    return r.f;
}

static C89ATOMIC_INLINE double c89atomic_compare_and_swap_f64(volatile double* dst, double expected, double replacement)
{
    c89atomic_if64 r;
    c89atomic_if64 e, d;
    e.f = expected;
    d.f = replacement;
    r.i = c89atomic_compare_and_swap_64((volatile c89atomic_uint64*)dst, e.i, d.i);
    return r.f;
}



#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
    #pragma GCC diagnostic pop  /* long long warnings with Clang. */
#endif

#if defined(__cplusplus)
}
#endif
#endif  /* c89atomic_h */

/*
This software is available as a choice of the following licenses. Choose
whichever you prefer.

===============================================================================
ALTERNATIVE 1 - Public Domain (www.unlicense.org)
===============================================================================
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

===============================================================================
ALTERNATIVE 2 - MIT No Attribution
===============================================================================
Copyright 2025 David Reid

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
