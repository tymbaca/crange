#include <stdio.h>
#include <stdlib.h>

// Assumes little endian
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    printf("%p: ", &b[i]);
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

int main() {
    int count = 10;
    int* arr = (int*)malloc(sizeof(int)*count);

    for (int i = 0; i < count; i++) {
        arr[i] = i;
    }

    uint8_t* bytearr = (uint8_t*)arr;
    
    /* printf("%d\n", arr[1]); */
    for (int i = 0; i < (count)*sizeof(int); i++) {
        printBits(1, &bytearr[i]);
    }
}

/* typedef struct { */
/* } */
