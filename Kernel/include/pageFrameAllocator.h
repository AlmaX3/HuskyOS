#pragma once
#include "bitmap.h"
#include "memory.h"
#include <stddef.h>
#include <stdint.h>
#include <stivale2.h>

class PageFrameAllocator {
    public:
    void ReadMemoryMap(stivale2_struct* info);
    Bitmap PageBitmap;
    void FreePage(void* Address);
    void FreePages(void* Address, uint64_t PageCount);
    void LockPage(void* Address);
    void LockPages(void* Address, uint64_t PageCount);
    void* RequestPage();
    void* RequestPages(int amount);
    uint64_t GetFreeRAM();
    uint64_t GetUsedRAM();
    uint64_t GetReservedRAM();


    private:
    void InitBitmap(size_t BitmapSize, void* BufferAddress);
    void ReservePage(void* Address);
    void ReservePages(void* Address, uint64_t PageCount);
    void UnreservePage(void* Address);
    void UnreservePages(void* Address, uint64_t PageCount);

};

extern PageFrameAllocator GlobalAllocator;