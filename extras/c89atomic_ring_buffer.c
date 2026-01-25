#ifndef c89atomic_ring_buffer_c
#define c89atomic_ring_buffer_c

#include "c89atomic_ring_buffer.h"

#include <string.h>
#include <assert.h>

#ifndef C89ATOMIC_RING_BUFFER_ASSERT
#define C89ATOMIC_RING_BUFFER_ASSERT(cond) assert(cond)
#endif

#ifndef C89ATOMIC_RING_BUFFER_COPY_MEMORY
#define C89ATOMIC_RING_BUFFER_COPY_MEMORY(dst, src, count) memcpy((dst), (src), (count))
#endif

#define C89ATOMIC_RING_BUFFER_OFFSET_PTR(p, offset) (void*)(((char*)(p)) + (offset))

/* BEG c89atomic_ring_buffer.c */
C89ATOMIC_RING_BUFFER_API void c89atomic_ring_buffer_init(c89atomic_uint32 capacity, c89atomic_uint32 stride, c89atomic_uint32 flags, void* pBuffer, c89atomic_ring_buffer* pRingBuffer)
{
    if (pRingBuffer == NULL) {
        return;
    }

    c89atomic_store_explicit_32(&pRingBuffer->head, 0, c89atomic_memory_order_relaxed);
    c89atomic_store_explicit_32(&pRingBuffer->tail, 0, c89atomic_memory_order_relaxed);
    pRingBuffer->capacity = 0;
    pRingBuffer->stride   = 0;
    pRingBuffer->flags    = 0;
    pRingBuffer->pBuffer  = NULL;

    if (pBuffer == NULL || stride == 0 || capacity == 0) {
        C89ATOMIC_RING_BUFFER_ASSERT(!"Ring buffer initialized with invalid values. It must have a valid buffer, stride and capacity.");
        return;
    }

    if (capacity > 0x7FFFFFFF) {
        C89ATOMIC_RING_BUFFER_ASSERT(!"Ring buffer capacity exceeds limit of 0x7FFFFFFF.");
        return;
    }

    pRingBuffer->capacity = capacity;
    pRingBuffer->stride   = stride;
    pRingBuffer->flags    = flags;
    pRingBuffer->pBuffer  = pBuffer;
}

static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_ring_buffer_calculate_length(c89atomic_uint32 head, c89atomic_uint32 tail, c89atomic_uint32 capacity)
{
    /*
    Our ring buffer encodes a "loop flag" in the head and tail cursors. The flag flip-flops as the cursor
    loops around and is basically just used to disambiguate the case when the head and tail are equal (are
    they the same because the buffer is empty, or because it's full).

    The head should always be ahead of the tail, and by no more than `capacity`. This algorithm asserts
    this rule. Logically it follows that the length is simply the head minus the tail. However, since the
    cursors loop, there are times when the tail will be greater than the head which will mess up the math.
    To make the math work we need to adjust the head to ensure it's always ahead of the tail.

    When the two cursors are on the same loop there's nothing complicated - just do the subtraction like
    normal. When they're on different loops we need only offset the head by the capacity before doing the
    subtraction. This can all be done without a branch.
    */
    #if 0
    {
        c89atomic_uint32 loopFlag = (head & 0x80000000) ^ (tail & 0x80000000);

        if (loopFlag) {
            return ((head & 0x7FFFFFFF) + capacity) - (tail & 0x7FFFFFFF);
        } else {
            return head - tail;
        }
    }
    #else
    {
        return ((head & 0x7FFFFFFF) + (capacity * (((head & 0x80000000) ^ (tail & 0x80000000)) >> 31))) - (tail & 0x7FFFFFFF);
    }
    #endif
}

static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_ring_buffer_calculate_remaining(c89atomic_uint32 head, c89atomic_uint32 tail, c89atomic_uint32 capacity)
{
    return capacity - c89atomic_ring_buffer_calculate_length(head, tail, capacity);
}

C89ATOMIC_RING_BUFFER_API size_t c89atomic_ring_buffer_map_produce(c89atomic_ring_buffer* pRingBuffer, size_t count, void** ppMappedBuffer)
{
    c89atomic_uint32 head;
    c89atomic_uint32 tail;
    c89atomic_uint32 remaining;

    if (ppMappedBuffer == NULL) {
        return 0;
    }

    *ppMappedBuffer = NULL;

    if (pRingBuffer == NULL) {
        return 0;
    }

    /*
    For the head, only the producer will be making modifications to it so we can just use relaxed. For the tail,
    it is indeed modified by the consumer, but since the producer does not actually have a data dependency on
    anything done by the consumer the tail is essentially just a cursor for us to determine how much data we can
    produce. It can therefore be relaxed as well.
    */
    head = c89atomic_load_explicit_32(&pRingBuffer->head, c89atomic_memory_order_relaxed);
    tail = c89atomic_load_explicit_32(&pRingBuffer->tail, c89atomic_memory_order_relaxed);

    /* Now we need to clamp the count to ensure it never goes beyond our capacity. */
    remaining = c89atomic_ring_buffer_calculate_remaining(head, tail, pRingBuffer->capacity);
    if (count > remaining) {
        count = remaining;
    }

    /* Our pointer will always just be where our head is pointing. */
    *ppMappedBuffer = C89ATOMIC_RING_BUFFER_OFFSET_PTR(pRingBuffer->pBuffer, (head & 0x7FFFFFFF) * pRingBuffer->stride);

    return count;
}

C89ATOMIC_RING_BUFFER_API void c89atomic_ring_buffer_unmap_produce(c89atomic_ring_buffer* pRingBuffer, size_t count)
{
    c89atomic_uint32 head;
    c89atomic_uint32 tail;

    if (pRingBuffer == NULL) {
        return;
    }

    head = c89atomic_load_explicit_32(&pRingBuffer->head, c89atomic_memory_order_relaxed);
    tail = c89atomic_load_explicit_32(&pRingBuffer->tail, c89atomic_memory_order_relaxed);

    C89ATOMIC_RING_BUFFER_ASSERT(count <= c89atomic_ring_buffer_calculate_remaining(head, tail, pRingBuffer->capacity));

    /* The tail is not modified. We load it for the benefit of the assert above. */
    (void)tail;

    /* If the buffer is not mirrored we need to copy any overflow to the start of the ring buffer. */
    if ((pRingBuffer->flags & C89ATOMIC_RING_BUFFER_FLAG_MIRRORED) == 0) {
        c89atomic_uint32 newHead = (head & 0x7FFFFFFF) + count;
        if (newHead  > pRingBuffer->capacity) {
            newHead -= pRingBuffer->capacity;
            C89ATOMIC_RING_BUFFER_COPY_MEMORY(pRingBuffer->pBuffer, C89ATOMIC_RING_BUFFER_OFFSET_PTR(pRingBuffer->pBuffer, pRingBuffer->capacity * pRingBuffer->stride), newHead * pRingBuffer->stride);
        }
    }

    /* Advance the head. */
    head += count;

    /* Check if the head has looped and adjust if so. */
    if ((head & 0x7FFFFFFF) >= pRingBuffer->capacity) {
        head -= pRingBuffer->capacity;  /* Get the index back into range. */
        head ^= 0x80000000;             /* Flip the loop flag. */
    }

    /*
    The consumer will be wanting to read from the buffer that we just wrote so we'll need to use
    release semantics here to ensure the consumer does not see the adjustment to the advanced head
    until after the data has been written.
    */
    c89atomic_store_explicit_32(&pRingBuffer->head, head, c89atomic_memory_order_release);
}

C89ATOMIC_RING_BUFFER_API size_t c89atomic_ring_buffer_map_consume(c89atomic_ring_buffer* pRingBuffer, size_t count, void** ppMappedBuffer)
{
    c89atomic_uint32 head;
    c89atomic_uint32 tail;
    c89atomic_uint32 length;

    if (ppMappedBuffer == NULL) {
        return 0;
    }

    *ppMappedBuffer = NULL;

    if (pRingBuffer == NULL) {
        return 0;
    }

    /*
    We're about to read data that was written by the producer. We'll need to use acquire semantics
    here for the head.
    */
    head = c89atomic_load_explicit_32(&pRingBuffer->head, c89atomic_memory_order_acquire);
    tail = c89atomic_load_explicit_32(&pRingBuffer->tail, c89atomic_memory_order_relaxed);

    /* Make sure we don't try to consume more than what we have available for consumption. */
    length = c89atomic_ring_buffer_calculate_length(head, tail, pRingBuffer->capacity);
    if (count > length) {
        count = length;
    }

    /* Our pointer will always just be where our tail is pointing. */
    *ppMappedBuffer = C89ATOMIC_RING_BUFFER_OFFSET_PTR(pRingBuffer->pBuffer, (tail & 0x7FFFFFFF) * pRingBuffer->stride);

    /*
    If the buffer is not mirrored we may need to copy some data from the start of the buffer to the overflow
    part so the caller has a contiguous block to work with.
    */
    if ((pRingBuffer->flags & C89ATOMIC_RING_BUFFER_FLAG_MIRRORED) == 0) {
        c89atomic_uint32 newTail = (tail & 0x7FFFFFFF) + count;
        if (newTail  > pRingBuffer->capacity) {
            newTail -= pRingBuffer->capacity;
            C89ATOMIC_RING_BUFFER_COPY_MEMORY(C89ATOMIC_RING_BUFFER_OFFSET_PTR(pRingBuffer->pBuffer, pRingBuffer->capacity * pRingBuffer->stride), pRingBuffer->pBuffer, newTail * pRingBuffer->stride);
        }
    }

    return count;
}

C89ATOMIC_RING_BUFFER_API void c89atomic_ring_buffer_unmap_consume(c89atomic_ring_buffer* pRingBuffer, size_t count)
{
    c89atomic_uint32 head;
    c89atomic_uint32 tail;

    if (pRingBuffer == NULL) {
        return;
    }

    /*
    When we first mapped the buffer with `map_consume()` we used acquire semantics for the head. Since we won't
    be touching any data that was produced between our map and unmap, we should be able to use relaxed here.
    */
    head = c89atomic_load_explicit_32(&pRingBuffer->head, c89atomic_memory_order_relaxed);
    tail = c89atomic_load_explicit_32(&pRingBuffer->tail, c89atomic_memory_order_relaxed);

    C89ATOMIC_RING_BUFFER_ASSERT(count <= c89atomic_ring_buffer_calculate_length(head, tail, pRingBuffer->capacity));

    /* The head is not modified. We load it for the benefit of the assert above. */
    (void)head;

    /* Advance the tail. */
    tail += count;

    /* Check if the tail has looped and adjust if so. */
    if ((tail & 0x7FFFFFFF) >= pRingBuffer->capacity) {
        tail -= pRingBuffer->capacity;  /* Get the index back into range. */
        tail ^= 0x80000000;             /* Flip the loop flag. */
    }

    /* The producer doesn't care what we do with the data in the buffer so we should be able to use relaxed semantics here for the tail. */
    c89atomic_store_explicit_32(&pRingBuffer->tail, tail, c89atomic_memory_order_relaxed);
}

C89ATOMIC_RING_BUFFER_API c89atomic_uint32 c89atomic_ring_buffer_length(const c89atomic_ring_buffer* pRingBuffer)
{
    c89atomic_uint32 head;
    c89atomic_uint32 tail;

    if (pRingBuffer == NULL) {
        return 0;
    }

    head = c89atomic_load_explicit_32(&pRingBuffer->head, c89atomic_memory_order_relaxed);
    tail = c89atomic_load_explicit_32(&pRingBuffer->tail, c89atomic_memory_order_relaxed);

    return c89atomic_ring_buffer_calculate_length(head, tail, pRingBuffer->capacity);
}

C89ATOMIC_RING_BUFFER_API c89atomic_uint32 c89atomic_ring_buffer_capacity(const c89atomic_ring_buffer* pRingBuffer)
{
    if (pRingBuffer == NULL) {
        return 0;
    }

    return pRingBuffer->capacity;
}
/* END c89atomic_ring_buffer.c */

#endif  /* c89atomic_ring_buffer_c */
