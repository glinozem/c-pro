#include "inc/mem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdalign.h>   /* alignof */
#include <stddef.h>     /* max_align_t */

/* ---- безопасные обёртки ---- */
static void oom_abort(size_t n) {
    fprintf(stderr, "FATAL: OOM requesting %zu bytes\n", n);
    abort();
}

void* xmalloc(size_t n) {
    if (n == 0) n = 1;
    void* p = malloc(n);
    if (!p) oom_abort(n);
    return p;
}

void* xcalloc(size_t n, size_t sz) {
    if (n == 0 || sz == 0) { n = 1; sz = 1; }
    if (n > SIZE_MAX / sz) oom_abort((size_t)-1);
    void* p = calloc(n, sz);
    if (!p) oom_abort(n * sz);
    return p;
}

void* xrealloc(void* p, size_t n) {
    if (n == 0) n = 1;
    void* q = realloc(p, n);
    if (!q) oom_abort(n);
    return q;
}

void xfree(void* p) { free(p); }

/* ---- арена ---- */
static size_t align_up(size_t x, size_t a) {
    assert(a && ((a & (a - 1)) == 0)); /* степень двойки */
    return (x + (a - 1)) & ~(a - 1);
}

int arena_init(mem_arena* A, size_t capacity) {
    if (!A) return -1;
    if (capacity == 0) capacity = 1;
    A->base = (unsigned char*)malloc(capacity);
    if (!A->base) return -1;
    A->cap = capacity;
    A->off = 0;
    return 0;
}

void arena_reset(mem_arena* A) { if (A) A->off = 0; }

void arena_free(mem_arena* A) {
    if (!A) return;
    free(A->base);
    A->base = NULL; A->cap = A->off = 0;
}

void* arena_alloc(mem_arena* A, size_t size, size_t align) {
    if (!A || !A->base) return NULL;
    if (align == 0) align = alignof(max_align_t);
    size_t off = align_up(A->off, align);
    if (size > SIZE_MAX - off) return NULL;
    if (off + size > A->cap)   return NULL;
    void* p = A->base + off;
    A->off = off + size;
    return p;
}

char* arena_strdup(mem_arena* A, const char* s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char* p = (char*)arena_alloc(A, n, alignof(char));
    if (!p) return NULL;
    memcpy(p, s, n);
    return p;
}
