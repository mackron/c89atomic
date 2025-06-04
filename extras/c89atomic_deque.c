#ifndef c89atomic_deque_c
#define c89atomic_deque_c

#include "c89atomic_deque.h"

#define c89atomic_deque_can_steal(x) (1)

/* BEG c89atomic_deque.c */
C89ATOMIC_DEQUE_API void c89atomic_deque_init(c89atomic_deque* pDeque)
{
    pDeque->head = 0;
    pDeque->tail = 0;
}

C89ATOMIC_DEQUE_API c89atomic_deque_result c89atomic_deque_push_tail(c89atomic_deque* pDeque, C89ATOMIC_DEQUE_T value)
{
    c89atomic_uint32 tail;
    c89atomic_uint32 head;

    tail = c89atomic_load_explicit_32(&pDeque->tail, c89atomic_memory_order_relaxed);
    head = c89atomic_load_explicit_32(&pDeque->head, c89atomic_memory_order_acquire);

    if ((tail - head) >= C89ATOMIC_DEQUE_CAP) {
        return C89ATOMIC_DEQUE_OUT_OF_MEMORY;  /* Queue is full */
    }

    c89atomic_store_explicit_ptr((volatile void**)&pDeque->buffer[tail & (C89ATOMIC_DEQUE_CAP - 1)], value, c89atomic_memory_order_relaxed);
    c89atomic_thread_fence(c89atomic_memory_order_release);
    c89atomic_store_explicit_32(&pDeque->tail, tail + 1, c89atomic_memory_order_relaxed);

    return C89ATOMIC_DEQUE_SUCCESS;
}

C89ATOMIC_DEQUE_API c89atomic_deque_result c89atomic_deque_take_tail(c89atomic_deque* pDeque, C89ATOMIC_DEQUE_T* pValue)
{
    c89atomic_uint32 tail;
    c89atomic_uint32 head;

    tail = c89atomic_load_explicit_32(&pDeque->tail, c89atomic_memory_order_relaxed);
    tail -= 1UL;  /* Can underflow, but should not matter here because we'll be masking with `C89ATOMIC_DEQUE_CAP`, and the conditional below is cast to a signed integer. See note below. */
    c89atomic_store_explicit_32(&pDeque->tail, tail, c89atomic_memory_order_relaxed);
    c89atomic_thread_fence(c89atomic_memory_order_seq_cst);
    head = c89atomic_load_explicit_32(&pDeque->head, c89atomic_memory_order_relaxed);

    /*
    NOTE:

    The Lê et al. paper has an unsigned integer underflow bug in it. Just above we are loading the tail and the
    subtracting 1 from it. When a deque is freshly initialized, most developers will intuitively set the head
    and tail to 0. As a result, the subtraction by 1 will result in the tail underflowing. In the paper, the
    conditional below is done against the unsigned values of the head and tail which will result in the
    algorithm thinking that the deque is filled when it's actually empty.

    I'm going to fix this by casting to a signed integer.
    */
    if ((c89atomic_int32)head <= (c89atomic_int32)tail) {
        /* Not empty. */
        C89ATOMIC_DEQUE_T x;

        x = c89atomic_load_explicit_ptr((volatile void**)&pDeque->buffer[tail & (C89ATOMIC_DEQUE_CAP - 1)], c89atomic_memory_order_relaxed);
        if (head == tail) {
            /* Last item in queue. In this case the head is moved forward rather than moving the tail backward. */
            c89atomic_bool raceResult = c89atomic_compare_exchange_strong_explicit_32(&pDeque->head, &head, head + 1, c89atomic_memory_order_seq_cst, c89atomic_memory_order_relaxed);

            /*
            Regardless of the output of the race we need to put the tail back to it's original location. In this branch (when we are taking
            the last item in the queue), we are moving the head forward rather than the tail backward which means we need to undo the subtraction
            by 1 we did earlier.
            */
            c89atomic_store_explicit_32(&pDeque->tail, tail + 1, c89atomic_memory_order_relaxed);

            /* Terminate early if we failed the race against a theif. */
            if (!raceResult) {
                return C89ATOMIC_DEQUE_NO_DATA_AVAILABLE;
            }
        }

        *pValue = x;
        return C89ATOMIC_DEQUE_SUCCESS;
    } else {
        /* Empty. */
        c89atomic_store_explicit_32(&pDeque->tail, tail + 1, c89atomic_memory_order_relaxed);
        return C89ATOMIC_DEQUE_NO_DATA_AVAILABLE;
    }
}

C89ATOMIC_DEQUE_API c89atomic_deque_result c89atomic_deque_take_head(c89atomic_deque* pDeque, C89ATOMIC_DEQUE_T* pValue)
{
    c89atomic_uint32 head;
    c89atomic_uint32 tail;

    head = c89atomic_load_explicit_32(&pDeque->head, c89atomic_memory_order_acquire);
    c89atomic_thread_fence(c89atomic_memory_order_seq_cst);
    tail = c89atomic_load_explicit_32(&pDeque->tail, c89atomic_memory_order_acquire);

    if ((c89atomic_int32)head < (c89atomic_int32)tail) {
        C89ATOMIC_DEQUE_T x;

        x = c89atomic_load_explicit_ptr((volatile void**)&pDeque->buffer[head & (C89ATOMIC_DEQUE_CAP - 1)], c89atomic_memory_order_relaxed);
        if (!c89atomic_deque_can_steal(x)) {
            return C89ATOMIC_DEQUE_CANCELLED;   /* Item is not stealable. */
        }

        if (!c89atomic_compare_exchange_strong_explicit_32(&pDeque->head, &head, head + 1, c89atomic_memory_order_seq_cst, c89atomic_memory_order_relaxed)) {
            return C89ATOMIC_DEQUE_CANCELLED;   /* Failed race.  */
        }

        *pValue = x;
        return C89ATOMIC_DEQUE_SUCCESS;
    } else {
        return C89ATOMIC_DEQUE_NO_DATA_AVAILABLE;  /* Empty. */
    }
}
/* END c89atomic_deque.c */

#endif /* c89atomic_deque_c */
