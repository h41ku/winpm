#ifndef __memory_h__
#define __memory_h__

#include "winapi.h"

#define memoryAlloc(size) VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE)
#define memoryFree(ptr) VirtualFree(ptr, 0, MEM_RELEASE)
void *memoryRealloc(void *ptr, int size, int newSize);
void memoryCopy(void *dst, void *src, int size);

#endif /* __memory_h__ */
