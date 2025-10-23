#include <cpro/mem.h>
#include <cpro/logger.h>
#include <string.h>
#include <assert.h>

int main(void) {
    /* обёртки */
    int* a = (int*)xmalloc(4 * sizeof(int));
    for (int i = 0; i < 4; ++i) a[i] = i;
    a = (int*)xrealloc(a, 8 * sizeof(int));
    for (int i = 4; i < 8; ++i) a[i] = i;
    xfree(a);

    int* z = (int*)xcalloc(16, sizeof(int));
    for (int i = 0; i < 16; ++i) assert(z[i] == 0);
    xfree(z);
    log_info("heap wrappers ok");

    /* арена */
    mem_arena A; assert(arena_init(&A, 1024) == 0);
    int* v = (int*)arena_alloc(&A, 10 * sizeof(int), 16);
    for (int i = 0; i < 10; ++i) v[i] = i * 2;

    char* s = arena_strdup(&A, "arena works");
    assert(strcmp(s, "arena works") == 0);

    void* big = arena_alloc(&A, 1u<<30, 8);
    assert(big == NULL);

    arena_reset(&A);
    arena_free(&A);
    log_info("arena ok");

    return 0;
}
