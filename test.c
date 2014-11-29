#include "my_malloc.h"
#define SBRK_SIZE 2048
int main(){
    void * mem;
    int i;
    for (i = 0; i < 5; i++)
        mem=my_malloc(SBRK_SIZE - sizeof(metadata_t));
}