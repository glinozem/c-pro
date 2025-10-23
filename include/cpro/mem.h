#ifndef CPRO_MEM_H
#define CPRO_MEM_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

    void* xmalloc(size_t n);
    void* xcalloc(size_t n, size_t sz);
    void* xrealloc(void* p, size_t n);
    void  xfree(void* p);

    typedef struct {
        unsigned char* base;
        size_t cap;
        size_t off;
    } mem_arena;

    int   arena_init(mem_arena* A, size_t capacity);
    void  arena_reset(mem_arena* A);
    void  arena_free (mem_arena* A);
    void* arena_alloc(mem_arena* A, size_t size, size_t align);
    char* arena_strdup(mem_arena* A, const char* s);

#ifdef __cplusplus
}
#endif
#endif /* CPRO_MEM_H */
