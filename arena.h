#include <stdlib.h>

typedef struct Arena Arena;

Arena* arena_create();
void* arena_alloc(Arena* arena, size_t size);
void arena_reset(Arena* arena);
void arena_free(Arena* arena);
