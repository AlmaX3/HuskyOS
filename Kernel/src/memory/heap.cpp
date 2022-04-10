#include <heap.h>
#include <PageTableManager.h>
#include <pageFrameAllocator.h>
#include <string.h>


void * heapStart;
void * heapEnd;
HeapSegHdr* LastHdr;
void initializeHeap(void *heapAddress, size_t pageCount){
    void * pos  = heapAddress;
    for (size_t i = 0; i < pageCount; i++)
    {
        GlobalPageTableManager.MapMemory(pos, GlobalAllocator.RequestPage());
        pos = (void*)((size_t)pos + 0x1000);
    }
    
    size_t heapLength = pageCount * 0x1000;

    heapStart = heapAddress;
    heapEnd = (void*)((size_t)heapStart + heapLength);
    HeapSegHdr* startSeg = (HeapSegHdr*)heapAddress;
    startSeg->length = heapLength - sizeof(HeapSegHdr);
    startSeg->next = 0;
    startSeg->last = 0;
    startSeg->free = true;
    LastHdr = startSeg;
    
}

void * malloc(size_t size){
    if(size % 0x10) {
        size -= (size%0x10);
        size += 0x10;
    }

    if(size == 0) return 0;

    HeapSegHdr* currentSgm = (HeapSegHdr*) heapStart;

    while (true)
    {
        if(currentSgm->free){
            if(currentSgm->length > size){
                currentSgm->Split(size);
                currentSgm->free = false;
                return (void*)((uint64_t)currentSgm + sizeof(HeapSegHdr));
            }
            if(currentSgm->length==size){
              currentSgm->free = false;
                return (void*)((uint64_t)currentSgm + sizeof(HeapSegHdr));
            }
        }
        if(currentSgm->next == 0) break;
        currentSgm = currentSgm->next;
    }
    ExpandHeap(size);
    return malloc(size);
    
}

void* calloc(int num, size_t size){
    return malloc(num*size);
}

void* realloc(void* addr, size_t size){


    HeapSegHdr* segment = (HeapSegHdr*)addr;
    if(segment->free){
        void* newptr = malloc(size);
        memcpy(newptr, segment, size);
        free(addr);
        return newptr;
    }

    return nullptr;
}

void free(void* addr) {
    HeapSegHdr* segment = (HeapSegHdr*)addr - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
}

HeapSegHdr* HeapSegHdr::Split(size_t splitLength) {
    if(splitLength < 0x10) return 0;
    int32_t splitSegLength =length - splitLength - (sizeof(HeapSegHdr));
    if(splitSegLength < 0x10) return 0;

    HeapSegHdr* newSplitHdr = (HeapSegHdr*)((size_t)this + splitLength+sizeof(HeapSegHdr));
    next->last = newSplitHdr;
    newSplitHdr->next = next;
    next = newSplitHdr;
    newSplitHdr->last = this;
    newSplitHdr->length=splitSegLength;
    newSplitHdr->free = free;
    length = splitLength;

    if(LastHdr == this) last = newSplitHdr;
    return newSplitHdr;
}

void ExpandHeap(size_t size) {
    if(size % 0x1000) {
        size -= size % 0x1000;
        size = 0x1000;
    }
    

    size_t pageCount = size / 0x1000;
    HeapSegHdr* newSegment = (HeapSegHdr*)heapEnd;

    for (size_t i = 0; i < pageCount; i++)
    {
        GlobalPageTableManager.MapMemory(heapEnd, GlobalAllocator.RequestPage());
        heapEnd = (void*)((size_t)heapEnd + 0x1000);
    }

    newSegment->free = true;
    newSegment->last = LastHdr;
    LastHdr->next=newSegment;
    LastHdr = newSegment;
    newSegment->next = 0;
    newSegment->length = size - sizeof(HeapSegHdr);
    newSegment->CombineBackward();

    
}

void HeapSegHdr::CombineForward(){
    if(next == 0) return;
    if(!next->free) return;

    if(next == LastHdr) LastHdr == this;
    if(next->next != 0) next->next->last = this;

    length = length + next->length + sizeof(HeapSegHdr);
}

void HeapSegHdr::CombineBackward() {
    if(last != 0 && last->free) last->CombineForward();
}