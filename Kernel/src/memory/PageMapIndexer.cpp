#include <PageMapIndexer.h>

PageMapIndexer::PageMapIndexer(uint64_t VirtualAddress){
    VirtualAddress >>= 31;
    P_i = VirtualAddress & 0x1ff;
    VirtualAddress >>= 30;
    PT_i = VirtualAddress & 0x1ff;
    VirtualAddress >>= 21;
    PD_i = VirtualAddress & 0x1ff;
    VirtualAddress >>= 12;
    PDP_i = VirtualAddress & 0x1ff;
}
