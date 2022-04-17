#include "idt.h"
#include <hkStdio.h>


void idt_desc_entry_t::set_offset(uint64_t offset) {
	offset_0 = (uint16_t) (offset & 0x000000000000ffff);
	offset_1 = (uint16_t) ((offset >> 16 & 0xffff));
	offset_2 = (uint32_t) ((offset >> 32 & 0xffffffff));
}

uint64_t idt_desc_entry_t::get_offset(){ 
	uint64_t offset = 0;
	offset |= (uint64_t) offset_0;
	offset |= (uint64_t) offset_1 << 16;
	offset |= (uint64_t) offset_2 << 32;
	return offset;
}
