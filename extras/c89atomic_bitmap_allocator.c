#ifndef c89atomic_bitmap_allocator_c
#define c89atomic_bitmap_allocator_c

#include "c89atomic_bitmap_allocator.h"

#include <assert.h>

/* Just a naive implementation just to get things working. A proper implementation would probably want to use a hardware instruction for this. */
static c89atomic_uint32 c89atomic_clz_32(c89atomic_uint32 x)
{
    c89atomic_uint32 i;

    for (i = 0; i < 32; i += 1) {
        if ((x & (0x80000000 >> i)) != 0) {
            return i;
        }
    }

    return i;
}


/* BEG c89atomic_bitmap_allocator.c */
C89ATOMIC_BITMAP_ALLOCATOR_API c89atomic_bitmap_allocator_result c89atomic_bitmap_allocator_init(void* pBitmap, size_t sizeInBits, c89atomic_bitmap_allocator* pAllocator)
{
    size_t i;

    if (pAllocator == NULL || pBitmap == NULL) {
        return C89ATOMIC_BITMAP_ALLOCATOR_INVALID_ARGS;
    }

    /* The size in bits must be a multiple of 31. */
    if ((sizeInBits & 31) != 0) {
        return C89ATOMIC_BITMAP_ALLOCATOR_INVALID_ARGS;
    }

    pAllocator->sizeInWords = sizeInBits / (sizeof(pAllocator->bitmap[0]) * 8);
    pAllocator->bitmap      = (c89atomic_uint32*)pBitmap;

    /* Initialize the bitmap to zero. */
    for (i = 0; i < pAllocator->sizeInWords; i += 1) {
        pAllocator->bitmap[i] = 0;
    }

    return C89ATOMIC_BITMAP_ALLOCATOR_SUCCESS;
}

C89ATOMIC_BITMAP_ALLOCATOR_API c89atomic_bitmap_allocator_result c89atomic_bitmap_allocator_alloc(c89atomic_bitmap_allocator* pAllocator, size_t* pIndex)
{
    size_t i;

    /* All we need to do is find the first set bit and return it's index. */
    for (i = 0; i < pAllocator->sizeInWords; i += 1) {
        c89atomic_uint32 oldWord;
        c89atomic_uint32 newWord;
        c89atomic_uint32 bitIndex;

        for (;;) {
            oldWord = c89atomic_load_explicit_32(&pAllocator->bitmap[i], c89atomic_memory_order_relaxed);
            if (oldWord == 0xFFFFFFFF) {
                break;  /* All bits are set, so continue to the next word. */
            }

            bitIndex = c89atomic_clz_32(~oldWord);
            assert(bitIndex < 32);

            newWord = oldWord | (0x80000000UL >> bitIndex);

            if (c89atomic_compare_exchange_strong_explicit_32(&pAllocator->bitmap[i], &oldWord, newWord, c89atomic_memory_order_acq_rel, c89atomic_memory_order_relaxed)) {
                *pIndex = (i * sizeof(pAllocator->bitmap[0]) * 8) + bitIndex;
                return C89ATOMIC_BITMAP_ALLOCATOR_SUCCESS;
            }
        }
    }

    /* If we reach here, it means we couldn't find a free bit. */
    return C89ATOMIC_BITMAP_ALLOCATOR_OUT_OF_MEMORY;
}

C89ATOMIC_BITMAP_ALLOCATOR_API void c89atomic_bitmap_allocator_free(c89atomic_bitmap_allocator* pAllocator, size_t index)
{
    c89atomic_uint32 wordIndex;
    c89atomic_uint32 bitIndex;
    c89atomic_uint32 oldWord;
    c89atomic_uint32 newWord;

    wordIndex = (c89atomic_uint32)((index & 0xFFFFFFFF) >> 5);   /* slot / 32 */
    bitIndex  = (c89atomic_uint32)((index & 0xFFFFFFFF) & 31);   /* slot % 32 */

    if (wordIndex >= pAllocator->sizeInWords) {
        assert(!"Index out of bounds in c89atomic_bitmap_allocator_free().");
        return;  /* Index out of bounds. */
    }

    do
    {
        oldWord = c89atomic_load_explicit_32(&pAllocator->bitmap[wordIndex], c89atomic_memory_order_relaxed);
        newWord = oldWord & ~(0x80000000UL >> bitIndex);

        /* Assert to catch a double free. */
        if ((oldWord & (0x80000000UL >> bitIndex)) == 0) {
            assert(!"Double free detected in c89atomic_bitmap_allocator_free().");
        }
    } while (!c89atomic_compare_exchange_strong_explicit_32(&pAllocator->bitmap[wordIndex], &oldWord, newWord, c89atomic_memory_order_acq_rel, c89atomic_memory_order_relaxed));
}
/* END c89atomic_bitmap_allocator.c */

#endif /* c89atomic_bitmap_allocator_c */
