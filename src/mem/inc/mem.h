#ifndef MEM_H
#define MEM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* безопасные обёртки */
    void* xmalloc(size_t n);
    void* xcalloc(size_t n, size_t sz);
    void* xrealloc(void* p, size_t n);
    void  xfree(void* p);

    /* простая арена (bump allocator) */
    typedef struct {
        unsigned char* base;
        size_t cap;
        size_t off;
    } mem_arena;

    int   arena_init(mem_arena* A, size_t capacity); /* 0=ok, -1=oom */
    void  arena_reset(mem_arena* A);                 /* off=0 */
    void  arena_free(mem_arena* A);                  /* освобождает буфер */
    void* arena_alloc(mem_arena* A, size_t size, size_t align);
    char* arena_strdup(mem_arena* A, const char* s);

#ifdef __cplusplus
}
#endif
#endif /* MEM_H */
