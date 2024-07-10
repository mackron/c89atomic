
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct thread_arg {
    void (*fn)(void *);
    void *arg;
};

extern void assert_expected(int res, int expected, const char *file, unsigned int line, const char *expr, const char *expected_str);

typedef struct thread_arg thread_arg;

extern uintptr_t thread_run(thread_arg *arg);

extern void thread_exit(uintptr_t value);

extern uintptr_t thread_join(uintptr_t handle);

extern void thread_sleep(int milliseconds);

extern void thread_yield(void);

#define CHK_EXPECTED(a, b) assert_expected(a, b, __FILE__, __LINE__, #a, #b)

#ifdef __cplusplus
}
#endif
