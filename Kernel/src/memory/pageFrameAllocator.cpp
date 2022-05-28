#include "string.h"
#include <bitmap.h>
#include <hkStdio.h>
#include <pageFrameAllocator.h>
#include <terminal.h>
#include <kernelGlobal.h>
#include <serial.h>
uint64_t FreeMemory;
uint64_t ReservedMemory;
uint64_t UsedMemory;
bool Initialized = false;
PageFrameAllocator GlobalAllocator;

void PageFrameAllocator::ReadMemoryMap(stivale2_struct* info) {
    if (Initialized)
        return;

    Initialized = true;

    stivale2_struct_tag_memmap *memory_map = (stivale2_struct_tag_memmap *)hKernel.stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    uint64_t MemoryMapEntries = memory_map->entries;
    void *LargestFreeMemorySegment = NULL;
    uint64_t LargestFreeMemorySegmentSize = 0;

    for (int i = 0; i < MemoryMapEntries; i++) {
        if (memory_map->memmap[i].type == STIVALE2_MMAP_USABLE || memory_map->memmap[i].type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE) { // type = EfiConventionalMemory
            debug("[Map entry %d]: base -> 0x%llx || length -> 0x%llx (%llu)\n", i, memory_map->memmap[i].base, memory_map->memmap[i].length, memory_map->memmap[i].length);
            if (memory_map->memmap[i].length > LargestFreeMemorySegmentSize) {
                debug("Checking if 0x%llx segment with the size of 0x%llx (%llu) is enough for bitmap.\n", (void *)memory_map->memmap[i].base, memory_map->memmap[i].length, memory_map->memmap[i].length);
                LargestFreeMemorySegment = (void *)memory_map->memmap[i].base;
                LargestFreeMemorySegmentSize = memory_map->memmap[i].length;
            }
        }
    }

    debug("Largest Free Mem Segment: 0x%llx\n", LargestFreeMemorySegment);
    debug("Largest Free Mem Segment size : 0x%llx\n", LargestFreeMemorySegmentSize);

    uint64_t MemorySize = MemFunc.GetAllFreeMemory(memory_map);
    FreeMemory = MemorySize;
    uint64_t BitmapSize = MemorySize / 4096 / 8 + 1;
    debug("Bitmap size: %llu\n", BitmapSize);
    InitBitmap(BitmapSize, LargestFreeMemorySegment);

    //ReservePages(0, MemorySize / 4096 + 1);
    for (int i = 0; i < memory_map->entries; i++){
		if (memory_map->memmap[i].type == STIVALE2_MMAP_RESERVED) { 
            debug("Reserving 0x%llx\n", memory_map->memmap[i].base);
			ReservePages((void*) memory_map->memmap[i].base, (memory_map->memmap[i].length / 0x1000) + 1);
		}
	}
    ReservePages(0, 0x100); // reserve between 0 and 0x100000
    LockPages(PageBitmap.Buffer, PageBitmap.Size / 4096 + 1);
}

void PageFrameAllocator::InitBitmap(size_t BitmapSize, void *BufferAddress) {
    PageBitmap.Size = BitmapSize;
    PageBitmap.Buffer = (uint8_t *)BufferAddress;
    for (int i = 0; i < BitmapSize; i++) {
        *(uint8_t *)(PageBitmap.Buffer + i) = 0;
    }
}

uint64_t PageBitmapIndex = 0;
void *PageFrameAllocator::RequestPage() {
   for (int i = 0; i < PageBitmapIndex; i++) {
		if(PageBitmap[i] == true) {
			continue;
		}
		PageBitmapIndex = i;
		break;
	}
	

	for (uint64_t x = PageBitmapIndex; x < PageBitmap.Size * 8; x++){
        if (PageBitmap[x] == true) continue;
        LockPage((void*)(x * 4096));
        return (void*)(x * 4096);
    }

    return NULL; // Page Frame Swap to file
}

void PageFrameAllocator::FreePage(void *Address) {
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == false)
        return;
    if (PageBitmap.Set(Index, false)) {
        FreeMemory += 4096;
        UsedMemory -= 4096;
        if (PageBitmapIndex > Index)
            PageBitmapIndex = Index;
    }
}


void* PageFrameAllocator::RequestPages(int amount){
	for (int i = 0; i < PageBitmapIndex; i++) {
		if(PageBitmap[i] == true) {
			continue;
		}

		PageBitmapIndex = i;
		break;
	}

	for (uint64_t x = PageBitmapIndex; x < PageBitmap.Size * 8; x++){
        if (PageBitmap[x] == true) {
			continue;
		}

		for (int i = 0; i < amount; i++) {
			if(PageBitmap[x + i] == true) goto next;
		}
		
        LockPages((void*)(x * 4096), amount);
        return (void*)(x * 4096);

	next:
		x += amount;
		continue;
    }

    return NULL; // Page Frame Swap to file
}

void PageFrameAllocator::FreePages(void *Address, uint64_t PageCount) {
    for (int t = 0; t < PageCount; t++) {
        FreePage((void *)((uint64_t)Address + (t * 4096)));
    }
}

void PageFrameAllocator::LockPage(void *Address) {
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == true)
        return;
    if (PageBitmap.Set(Index, true)) {
        FreeMemory -= 4096;
        UsedMemory += 4096;
    }
}

void PageFrameAllocator::LockPages(void *Address, uint64_t PageCount) {
    for (int t = 0; t < PageCount; t++) {
        LockPage((void *)((uint64_t)Address + (t * 4096)));
    }
}

void PageFrameAllocator::UnreservePage(void *Address) {
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == false)
        return;
    if (PageBitmap.Set(Index, false)) {
        FreeMemory += 4096;
        ReservedMemory -= 4096;
        if (PageBitmapIndex > Index)
            PageBitmapIndex = Index;
    }
}

void PageFrameAllocator::UnreservePages(void *Address, uint64_t PageCount) {
    for (int t = 0; t < PageCount; t++) {
        UnreservePage((void *)((uint64_t)Address + (t * 4096)));
    }
}

void PageFrameAllocator::ReservePage(void *Address) {
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == true)
        return;
    if (PageBitmap.Set(Index, true)) {
        FreeMemory -= 4096;
        ReservedMemory += 4096;
    }
}

void PageFrameAllocator::ReservePages(void *Address, uint64_t PageCount) {
    for (int t = 0; t < PageCount; t++) {
        ReservePage((void *)((uint64_t)Address + (t * 4096)));
    }
}

uint64_t PageFrameAllocator::GetFreeRAM() {
    return FreeMemory;
}
uint64_t PageFrameAllocator::GetUsedRAM() {
    return UsedMemory;
}
uint64_t PageFrameAllocator::GetReservedRAM() {
    return ReservedMemory;
}