#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

struct HeapSegHdr {
    size_t length;
    HeapSegHdr* next;
    HeapSegHdr* last;
    bool free;
    void CombineForward();
    void CombineBackward();
    HeapSegHdr* Split(size_t splitLength);
};

void initializeHeap(void *heapAddress, size_t pageCount);

void * malloc(size_t size);
void* calloc(int num, size_t size);
void* realloc(void* addr, size_t size);
void free(void* addr);

void ExpandHeap(size_t length);