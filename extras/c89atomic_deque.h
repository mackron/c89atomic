/*
A double-ended queue (deque). This implements "Lê et al., Correct and Efficient Work-Stealing for Weak Memory Models (PPoPP 2013)".

This is lock-free and thread-safe, but with rules. Pushing and popping to/from the tail can only be done by one thread (usually
the thread that owns the deque). Stealing from the head can be done by any thread.

The capacity of the deque must be a power of 2, and it cannot be resized. If this is not suitable you'll need to repurpose this
code to suit your requirements, or use something else.

Use c89atomic_deque_push_tail() to push to the tail, c89atomic_deque_take_tail() to pop from the tail, and c89atomic_deque_take_head()
to steal from the head. You can initialize the deque with c89atomic_deque_init(), but it is not strictly necessary so long as you
zero out the head and tail before using it.

This code will do validation against the capacity of the deque, but will not validate function parameters. It's up to you to ensure
valid pointers are passed to all function calls.

I've designed this with the intentenion that it be amalgamated into other projects. You may need to adapt the code to suit your
specific requirements. The data type of the deque is defined by `C89ATOMIC_DEQUE_T` which defaults to `void*`. As a result the
`*_ptr` variants of atomic operations are used. If you want to use a non-pointer type, you'll need to adapt the code appropriately.
*/
#ifndef c89atomic_deque_h
#define c89atomic_deque_h

#include "../c89atomic.h"

#ifndef C89ATOMIC_DEQUE_API
#define C89ATOMIC_DEQUE_API
#endif

typedef enum
{
    C89ATOMIC_DEQUE_SUCCESS = 0,
    C89ATOMIC_DEQUE_OUT_OF_MEMORY,          /* Can be returned when pushing and the queue is full. */
    C89ATOMIC_DEQUE_NO_DATA_AVAILABLE,      /* Can be returned when the queue is empty during a take or steal. */
    C89ATOMIC_DEQUE_CANCELLED               /* Can be returned in the event of a data race when stealing. */
} c89atomic_deque_result;

/* Capacity must be power of 2.*/
#ifndef C89ATOMIC_DEQUE_CAP
#define C89ATOMIC_DEQUE_CAP 256
#endif

#ifndef C89ATOMIC_DEQUE_T
#define C89ATOMIC_DEQUE_T void*
#endif

/* BEG c89atomic_deque.h */
typedef struct c89atomic_deque
{
    c89atomic_uint32  head;
    c89atomic_uint32  tail;
    C89ATOMIC_DEQUE_T buffer[C89ATOMIC_DEQUE_CAP];
} c89atomic_deque;

C89ATOMIC_DEQUE_API void c89atomic_deque_init(c89atomic_deque* pDeque);
C89ATOMIC_DEQUE_API c89atomic_deque_result c89atomic_deque_push_tail(c89atomic_deque* pDeque, C89ATOMIC_DEQUE_T value);
C89ATOMIC_DEQUE_API c89atomic_deque_result c89atomic_deque_take_tail(c89atomic_deque* pDeque, C89ATOMIC_DEQUE_T* pValue);
C89ATOMIC_DEQUE_API c89atomic_deque_result c89atomic_deque_take_head(c89atomic_deque* pDeque, C89ATOMIC_DEQUE_T* pValue);  /* Steal an item with this. */
/* END c89atomic_deque.h */

#endif /* c89atomic_deque_h */
