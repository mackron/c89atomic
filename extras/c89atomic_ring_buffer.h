#ifndef c89atomic_ring_buffer_h
#define c89atomic_ring_buffer_h

#include "../c89atomic.h"
#include <stddef.h>

#ifndef C89ATOMIC_RING_BUFFER_API
#define C89ATOMIC_RING_BUFFER_API
#endif

#ifndef C89ATOMIC_RING_BUFFER_CACHE_LINE_SIZE
    #if defined(__powerpc64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
    #define C89ATOMIC_RING_BUFFER_CACHE_LINE_SIZE   128
    #elif defined(__APPLE__) && (defined(__aarch64__) || defined(__arm64__)) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED)
    #define C89ATOMIC_RING_BUFFER_CACHE_LINE_SIZE   128
    #else
    #define C89ATOMIC_RING_BUFFER_CACHE_LINE_SIZE   64
    #endif
#endif

/*
Ring Buffer
===========
The ring buffer is single producer, single consumer and lock-free. It is thread-safe so long as the
single producer, single consumer requirement is met. If you want multiple producers or consumers
you need to use a different data structure or do your own mutual exclusion.

To initialize the ring buffer, use `c89atomic_ring_buffer_init()`. You need to supply the capacity
of the buffer in elements, the size of an element in bytes (the `stride`) and a pointer to a buffer
that acts as the underlying data store of the ring buffer.

The capacity is limited to 0x7FFFFFFF. The reason is that the most significant bit is used as a
flag to disambiguate whether or not the ring buffer is empty or full when the head and tail cursors
are sitting on the same slot. The use of a 32-bit capacity instead of a `size_t` is to ensure the
ring buffer can be lock-free on 32-bit hardware. An assertion failure will result when initializing
if you specify a capacity that exceeds this limit.

You need to allocate the internal buffer yourself. The buffer must be twice the size of the
capacity multiplied by the stride:

    bufferSize = 2 * capacity * stride;

Below is an example for initializing a ring buffer:

    c89atomic_uint32 buffer[16 * 2];    // 2x because the buffer must be twice the size.
    c89atomic_ring_buffer rb;
    c89atomic_ring_buffer_init(16, sizeof(buffer[0]), 0, buffer, &rb);

You can use a mirrored buffer where the second half of the buffer is mapped to the same physical
memory as the first half. If you do this, use the `MIRRORED` flag at initialization time. Below is
an example:

    void* pMirroredBuffer = ...;    // Use a platform-specific routine to allocate this.
    c89atomic_ring_buffer rb;
    c89atomic_ring_buffer_init(capacity, stride, C89ATOMIC_RING_BUFFER_FLAG_MIRRORED, pMirroredBuffer, &rb);

The capacity and stride must not be zero.

Reading and writing is done through a mapping system. To produce (write) data, do something like
the following:

    // Map first.
    void* pMappedBuffer;
    c89atomic_uint32 count = 16;
    c89atomic_uint32 mappedCount = c89atomic_ring_buffer_map_produce(&rb, count, &pMappedBuffer);

    // Copy your data.
    memcpy(pMappedBuffer, pDataToWrite, mappedCount * sizeof(my_element));

    // Unmap.
    c89atomic_ring_buffer_unmap_produce(&rb, mappedCount);

The first thing you need to do is map a pointer that you will then later write into. Then when
you've finished writing data, you unmap the data. When you map the buffer, you specify how many
elements you want to write into the buffer and it will return how many were actually mapped. If it
is less that what you requested it means there is not enough room in the buffer. The data is
"committed" when you unmap. You can unmap less than the number of elements that were mapped, but
you must never attempt to unmap more than were mapped.

The exact same process applies when consuming (reading) data:

    // Map first.
    void* pMappedBuffer;
    c89atomic_uint32 count = 16;
    c89atomic_uint32 mappedCount = c89atomic_ring_buffer_map_consume(&rb, count, &pMappedBuffer);

    // Do something with your data.
    do_something(pMappedBuffer, mappedCount);

    // Unmap.
    c89atomic_ring_buffer_unmap_consume(&rb, mappedCount);

When mapping, it's possible that the range straddles the looping point. This is where whether or
not the buffer is mirrored becomes relevant. When it's mirrored there is nothing of note to
mention, but when the buffer is *not* mirrored, an extra data copy is employed at the looping
point. This is to allow the caller to work on a contiguous block of memory. When a consume buffer
is mapped a copy of the looped part of the data will be made and placed into the overflow portion
of the internal buffer before returning. The same idea applies when producing data, but it's done
when unmapping, and the copy is the other way around.

You can get an approximate count of the number of elements currently in the buffer by calling the
`c89atomic_ring_buffer_length()` function. In practice, a ring buffer will usually be operated on
by two different threads simultaneously which means the returned value may already be out of date
by the time it returns. You should therefore be careful with how you use this function in a
multithreaded scenario. In addition, the result is not well defined if you call it from a thread
other than the producer or consumer thread so therefore you should avoid calling this function from
a third thread.
*/

/* BEG c89atomic_ring_buffer.h */
#define C89ATOMIC_RING_BUFFER_FLAG_MIRRORED (1 << 0)    /* When set the buffer is mirrored at capacity * stride. */

typedef struct c89atomic_ring_buffer
{
    c89atomic_uint32 head;      /* Atomic. Most significant bit is a loop flag. When the head is equal to the tail (including the flag) it means the buffer is empty. If the only difference is the flag, it means the buffer is full. */
    #if C89ATOMIC_RING_BUFFER_CACHE_LINE_SIZE > 4
    c89atomic_uint8 pad0[C89ATOMIC_RING_BUFFER_CACHE_LINE_SIZE - 4];   /* The head will be modified by the producer, but untouched by the consumer. The reverse is true for the tail. The idea here is to ensure the head and tail are on their own cache lines thereby improving cache coherency. */
    #endif
    c89atomic_uint32 tail;      /* Atomic. Most significant bit is a loop flag. */
    #if C89ATOMIC_RING_BUFFER_CACHE_LINE_SIZE > 4
    c89atomic_uint8 pad1[C89ATOMIC_RING_BUFFER_CACHE_LINE_SIZE - 4];
    #endif
    c89atomic_uint32 capacity;  /* Capacity of the buffer, in elements. */
    c89atomic_uint32 stride;    /* Size of an element in bytes. */
    c89atomic_uint32 flags;
    void* pBuffer;              /* Must be twice the size of capacity * stride. */
} c89atomic_ring_buffer;

C89ATOMIC_RING_BUFFER_API void c89atomic_ring_buffer_init(c89atomic_uint32 capacity, c89atomic_uint32 stride, c89atomic_uint32 flags, void* pBuffer, c89atomic_ring_buffer* pRingBuffer);   /* Buffer must be `2 * capacity * stride`. That is twice the capacity. You can use a mirrored buffer, in which case specify the C89ATOMIC_RING_BUFFER_FLAG_MIRRORED flag. */
C89ATOMIC_RING_BUFFER_API c89atomic_uint32 c89atomic_ring_buffer_map_produce(c89atomic_ring_buffer* pRingBuffer, c89atomic_uint32 count, void** ppMappedBuffer);    /* Returns the number of elements actually mapped. */
C89ATOMIC_RING_BUFFER_API void c89atomic_ring_buffer_unmap_produce(c89atomic_ring_buffer* pRingBuffer, c89atomic_uint32 count);
C89ATOMIC_RING_BUFFER_API c89atomic_uint32 c89atomic_ring_buffer_map_consume(c89atomic_ring_buffer* pRingBuffer, c89atomic_uint32 count, void** ppMappedBuffer);    /* Returns the number of elements actually mapped. */
C89ATOMIC_RING_BUFFER_API void c89atomic_ring_buffer_unmap_consume(c89atomic_ring_buffer* pRingBuffer, c89atomic_uint32 count);
C89ATOMIC_RING_BUFFER_API c89atomic_uint32 c89atomic_ring_buffer_length(const c89atomic_ring_buffer* pRingBuffer);      /* Returns the number of elements currently in the ring buffer. Should only be called from the producer or consumer thread. If something is in the middle of producing or consuming data on the ring buffer than the returned value may already be out of date. */
C89ATOMIC_RING_BUFFER_API c89atomic_uint32 c89atomic_ring_buffer_capacity(const c89atomic_ring_buffer* pRingBuffer);
/* END c89atomic_ring_buffer.h */

#endif  /* c89atomic_ring_buffer_h */
