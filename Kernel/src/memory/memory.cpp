#include "../../include/memory.h"

uint64_t MemoryFunctions::GetAllFreeMemory(stivale2_struct_tag_memmap *memmap_str_tag) {
    uint64_t Mems = 0;
    for (size_t i = 0; i < memmap_str_tag->entries; i++) {
            Mems += memmap_str_tag->memmap[i].length;
    }
    return Mems;
}

MemoryFunctions MemFunc;