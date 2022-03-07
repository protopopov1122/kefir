#define NO_TRAMPOLINES

// Missing constants
#define __INT_MAX__ 0x7FFFFFFF
#define __LONG_MAX__ 0x7FFFFFFFFFFFFFFFl
#define __LONG_LONG_MAX__ 0x7FFFFFFFFFFFFFFFl
#define __CHAR_BIT__ 8
#define __FLT_MIN__    \
    (((union {         \
         long l;       \
         float f;      \
     }){.l = 8388608}) \
         .f)
#define __FLT_MAX__       \
    (((union {            \
         long l;          \
         float f;         \
     }){.l = 2139095039}) \
         .f)
#define __SIZEOF_INT__ sizeof(int)
#define __SIZEOF_LONG_LONG__ sizeof(long long)
#define __FUNCTION__ __func__

// Missing types
#define __inline__ inline
#define __SIZE_TYPE__ unsigned long
#define __UINTPTR_TYPE__ unsigned long
#define __INTPTR_TYPE__ long
#define __label__ void *
#define __restrict restrict

// Missing built-ins
#define __builtin_memset(p, v, n) memset(p, v, n)
#define __builtin_memcpy(d, s, n) memcpy(d, s, n)
#define __builtin_memcmp(d, s, n) memcmp(d, s, n)
#define __builtin_expect(e, x) (e)
#define __builtin_trap() abort()
#define __builtin_strcpy(d, s) strcpy(d, s)
#define __builtin_strncpy(d, s, n) strncpy(d, s, n)
#define __builtin_strcmp(d, s) strcmp(d, s)
#define __builtin_strlen(s) strlen(s)
#define __builtin_signbit(n) signbit(n)
#define __builtin_abort() abort()
#define __builtin_printf printf
#define __builtin_sprintf sprintf
#define __builtin_prefetch(addr, ...) \
    do {                              \
        (void) (addr);                \
    } while (0)
#define __builtin_longjmp(env, status) longjmp(env, status)
#define __builtin_setjmp(env) setjmp(env, status)
#define __builtin_exit() exit()
#define __builtin_unreachable() abort()
#define __builtin_abs(n) abs(n)
#define __builtin_ffs(n) ffs(n)
