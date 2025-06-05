/*
This does not do input parameter validation for null pointers. You must do this manually before calling any functions.
*/
#ifndef c89atomic_bitmap_allocator_h
#define c89atomic_bitmap_allocator_h

#include "../c89atomic.h"

#ifndef C89ATOMIC_BITMAP_ALLOCATOR_API
#define C89ATOMIC_BITMAP_ALLOCATOR_API
#endif

typedef enum
{
    C89ATOMIC_BITMAP_ALLOCATOR_SUCCESS = 0,
    C89ATOMIC_BITMAP_ALLOCATOR_INVALID_ARGS,    
    C89ATOMIC_BITMAP_ALLOCATOR_OUT_OF_MEMORY    /* Can be returned when allocating. */
} c89atomic_bitmap_allocator_result;


/* BEG c89atomic_bitmap_allocator.h */
typedef struct c89atomic_bitmap_allocator
{
    c89atomic_uint32* bitmap;
    size_t sizeInWords;
} c89atomic_bitmap_allocator;

C89ATOMIC_BITMAP_ALLOCATOR_API c89atomic_bitmap_allocator_result c89atomic_bitmap_allocator_init(void* pBitmap, size_t sizeInBits, c89atomic_bitmap_allocator* pAllocator);
C89ATOMIC_BITMAP_ALLOCATOR_API c89atomic_bitmap_allocator_result c89atomic_bitmap_allocator_alloc(c89atomic_bitmap_allocator* pAllocator, size_t* pIndex);
C89ATOMIC_BITMAP_ALLOCATOR_API void c89atomic_bitmap_allocator_free(c89atomic_bitmap_allocator* pAllocator, size_t index);
/* END c89atomic_bitmap_allocator.h */

#endif /* c89atomic_bitmap_allocator_h */
