#include <stdlib.h>
#include <stdio.h>
#include "arena.h"

int main() {
    Arena* arena = arena_create();
    char* c = arena_alloc(arena, 1);
    c[0] = 127;
    c[1] = 127;
    c[2] = 127;
    c[3] = 127;
    c[4] = 127;
    c[5] = 127;
    c[6] = 127;
    c[7] = 127;

    printf("alloc 1 byte\n");

    int count = 50;
    int* nums = arena_alloc(arena, sizeof(int)*count);
    printf("alloc array\n");
    for (int i = 0; i < count; i++) {
        nums[i] = i;
    }
    printf("filled array\n");

    for (int i = 0; i < count; i++) {
        nums[i] = i;
    }
    int* arena_as_nums = (int*)arena->start;
    for (int i = 0; i < count; i++) {
        printf("%d:\t %d\n", i, arena_as_nums[i]);
    }

    return 0;
}
