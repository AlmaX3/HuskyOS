#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stivale2.h>
#include <paging.h>


#define HIGHER_HALF_DATA_LV5	0xFF00000000000000UL
#define HIGHER_HALF_DATA_LV4	0xFFFF800000000000UL
#define HIGHER_HALF_CODE	0xFFFFFFFF80000000UL

#define PAGE_SIZE		    4096
#define TABLES_PER_DIRECTORY	    512
#define PAGES_PER_TABLE		    512	// TODO: do I need this???

#define KB_TO_PAGES(kb)		    (((kb) * 1024) / PAGE_SIZE)
#define ALIGN_DOWN(addr, align)	    ((addr) & ~((align)-1))
#define ALIGN_UP(addr, align)	    (((addr) + (align)-1) & ~((align)-1))

#define IS_PAGE_ALIGNED(num)	    ((num % PAGE_SIZE) == 0)

// get cr4 and return la57 = bit 12
static inline bool is_la57_enabled(void)
{
    uint64_t cr4;

    asm volatile("mov %%cr4, %0" : "=rax"(cr4));

    return (cr4 >> 12) & 1;
}

/* those functions are important, as KnutOS is a higher half kernel */

static inline uintptr_t phys_to_higher_half_data(uintptr_t address)
{
    if (is_la57_enabled())
	return HIGHER_HALF_DATA_LV5 + address;

    return HIGHER_HALF_DATA_LV4 + address;
}

static inline uintptr_t phys_to_higher_half_code(uintptr_t address)
{
    return HIGHER_HALF_CODE + address;
}

static inline uintptr_t higher_half_data_to_phys(uintptr_t address)
{
    if (is_la57_enabled())
	return address - HIGHER_HALF_DATA_LV5;

    return address - HIGHER_HALF_DATA_LV4;
}

static inline uintptr_t higher_half_code_to_phys(uintptr_t address)
{
    return address - HIGHER_HALF_CODE;
}


class MemoryFunctions {
    public: 
        uint64_t GetAllFreeMemory(stivale2_struct_tag_memmap* memmap_str_tag);
};



extern MemoryFunctions MemFunc;