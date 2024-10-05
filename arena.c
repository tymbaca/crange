#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Arena {
    void* start;
    size_t offset;
    size_t size;
    bool full;
    struct Arena* next;
} Arena;

Arena* arena_create(size_t size) {
    Arena* arena = malloc(sizeof(Arena));
    arena->start = malloc(size);
    arena->offset = 0;
    arena->size = size;
    arena->next = NULL;

    return arena;
}

void* _put_and_shift(Arena* arena, size_t size) {
    void* ptr = arena->start + arena->offset;
    arena->offset += size;
    return ptr;
}

bool _can_fit(Arena* arena, size_t size) {
    return arena->offset + size <= arena->size;
}

void* arena_alloc(Arena* arena, size_t size) {
    if (size > arena->size) {
        return NULL;
    }

    if (!_can_fit(arena, size) && arena->next != NULL) {
        return arena_alloc(arena->next, size);
    }

    // ok and full
    if (arena->offset + size == arena->size) {
        arena->next = arena_create(arena->size);
        return _put_and_shift(arena, size);
    }
    // doesn't fit
    if (arena->offset + size > arena->size) {
        arena->next = arena_create(arena->size);
        return arena_alloc(arena->next, size);
    }

    return _put_and_shift(arena, size);
}

void arena_destroy(Arena* arena) {
    // TODO
}

int main() {
    Arena* arena1 = arena_create(16);
    char* str1 = arena_alloc(arena1, 6);
    strcpy(str1, "hello"); // 6th is \0
    char* str2 = arena_alloc(arena1, 6);
    strcpy(str2, "world"); // 6th is \0
    char* str3 = arena_alloc(arena1, 4);
    strcpy(str3, "wow"); // 4th is \0
    char* str4 = arena_alloc(arena1, 16);
    strcpy(str1, "hello world wow"); // 16th is \0

    Arena* arena2 = arena_create(16);
    str1 = arena_alloc(arena2, 6);
    strcpy(str1, "hello"); // 6th is \0
    str2 = arena_alloc(arena2, 6);
    strcpy(str2, "world"); // 6th is \0
    str3 = arena_alloc(arena2, 5);
    strcpy(str3, "FUCK"); // 5th is \0
    str4 = arena_alloc(arena2, 4);
    strcpy(str3, "wow"); // 4th is \0
}

/* 
 * 0123456789...
 * -------- 
 * ----
 *     +
 * size = 8 
 * offset = 4
 *
 * want 4 - ok and full
 * -------- 
 * ----++++
 *
 *  */
