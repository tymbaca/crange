#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef struct Arena {
    void* start;
    size_t offset;
    size_t size;
    struct Arena* next;
} Arena;

size_t _default_arena_size = 10 * 1024;

Arena* _arena_create_size(size_t size) {
    Arena* arena = malloc(sizeof(Arena));
    arena->start = malloc(size);
    arena->offset = 0;
    arena->size = size;
    arena->next = NULL;

    return arena;
}

Arena* arena_create() {
    return _arena_create_size(_default_arena_size);
}

void* _put_and_shift(Arena* arena, size_t size) {
    void* ptr = (char*)arena->start + arena->offset;
    arena->offset += size;
    return ptr;
}

bool _can_fit(Arena* arena, size_t size) {
    return arena->offset + size <= arena->size;
}

void* arena_alloc(Arena* arena, size_t size) {
    assert(arena != NULL);

    if (!_can_fit(arena, size)) {
        if (arena->next == NULL) {
            // in case the wanted size is bigger than _default_arena_size
            if (size > _default_arena_size) {
                arena->next = _arena_create_size(size);
            } else {
                arena->next = _arena_create_size(_default_arena_size);
            }
        }
        return arena_alloc(arena->next, size);
    }

    return _put_and_shift(arena, size);
}

void arena_reset(Arena* arena) {
    arena->offset = 0;

    if (arena->next) {
        arena_reset(arena->next);
    }
}

void arena_free(Arena* arena) {
    if (arena == NULL) return;

    if (arena->next) {
        arena_free(arena->next);
    }

    free(arena->start);
    free(arena);
}

int main() {
    Arena* arena = arena_create(16);
    char* str1 = arena_alloc(arena, 6);
    strcpy(str1, "hello"); // 6th is \0
    char* str2 = arena_alloc(arena, 6);
    strcpy(str2, "world"); // 6th is \0
    char* str3 = arena_alloc(arena, 4);
    strcpy(str3, "wow"); // 4th is \0
    char* str4 = arena_alloc(arena, 16);
    strcpy(str4, "hello world wow"); // 16th is \0

    arena_reset(arena);

    str1 = arena_alloc(arena, 6);
    strcpy(str1, "HELLO"); // 6th is \0
    str2 = arena_alloc(arena, 6);
    strcpy(str2, "WORLD"); // 6th is \0
    str3 = arena_alloc(arena, 5);
    strcpy(str3, "FUCK"); // 5th is \0
    str4 = arena_alloc(arena, 4);
    strcpy(str4, "WOW"); // 4th is \0

    arena_free(arena);
}
