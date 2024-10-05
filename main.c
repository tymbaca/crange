#include <stdlib.h>
#include <stdio.h>
#include "arena.h"

int main() {
    size_t i = sizeof(void*);
    printf("allignment: %lu", i);
    return 0;
}
