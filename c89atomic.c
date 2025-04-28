/*
This file is only needed for the global lock, which is only needed for architectures that
don't natively support a particular atomic operation. Most applications will not need this,
but if you get errors about c89atomic_global_lock not being defined, you need to include
this file in your project.
*/
#ifndef c89atomic_c
#define c89atomic_c

#include "c89atomic.h"

c89atomic_spinlock c89atomic_global_lock = 0;

#endif /* c89atomic_h*/
