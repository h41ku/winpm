#ifndef __memory_c__
#define __memory_c__

#include "memory.h"

void *memoryRealloc(void *ptr, int size, int newSize) {
    void *dst = memoryAlloc(newSize);
    memoryCopy(dst, ptr, size);
    memoryFree(ptr);
    return dst;
}

void memoryCopy(void *dst, void *src, int size) {
    int i = 0;
    for (i = 0; i < size; i ++)
        ((LPBYTE) dst)[i] = ((LPBYTE) src)[i];
}

#endif /* __memory_c__ */
