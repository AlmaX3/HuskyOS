#include <kernelGlobal.h>

static uint8_t stack[8192];

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0},
    .flags = 0};

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&terminal_hdr_tag},
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0};

__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uintptr_t)&framebuffer_hdr_tag};

extern uint64_t _KernelStart;
extern uint64_t end;

void Kernel::makeGDT() {
    setup_gdt();
    HuskyStandardOutput.kprint("[ GDT STATUS ] INITIALIZED\n");
   
    
}

void Kernel::KernelStart(struct stivale2_struct *stivale2_struct) {
    cpu_init();
    struct stivale2_struct_tag_terminal *term_str_tag;
    term_str_tag = (stivale2_struct_tag_terminal *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);
    TextMode.initializeTextMode(term_str_tag);
    HuskyStandardOutput.kprint("Husky Kernel Started!\n");

    struct stivale2_struct_tag_memmap *memmap_str_tag;
    memmap_str_tag = (stivale2_struct_tag_memmap *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadMemoryMap(stivale2_struct);

    uint64_t kernel_size = (uint64_t)&end - (uint64_t)&_KernelStart;
    uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernel_pages);
    HuskyStandardOutput.kprint("Kernel size: %llu (pages: %llu)\n", kernel_size, kernel_pages);

    PageTable *PML4Phys = (PageTable *)GlobalAllocator.RequestPage();
    PageTable *PML4 = (PageTable *)((uint64_t)PML4Phys + 0xffff800000000000);
    memset(PML4, 0, 0x1000);

    GlobalPageTableManager = PageTableManager(PML4Phys);

    for (uint64_t t = 0; t < MemFunc.GetAllFreeMemory(memmap_str_tag); t += 0x1000) {
        GlobalPageTableManager.MapMemory((void *)t, (void *)t);
    }

    for (uint64_t i = 0; i < MemFunc.GetAllFreeMemory(memmap_str_tag) / 0x1000; i++) {
        uintptr_t physaddr = i * 0x1000;
        GlobalPageTableManager.MapMemory((void *)physaddr + 0xffff800000000000, (void *)physaddr);
    }

    stivale2_struct_tag_kernel_base_address* kernelBaseAddrStruct = (stivale2_struct_tag_kernel_base_address *)(stivale2_get_tag(stivale2_struct ,STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID));
    stivale2_struct_tag_pmrs * pmrsStruct = (stivale2_struct_tag_pmrs *)(stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_PMRS_ID));
    for (uint64_t pmrIndex = 0; pmrIndex < pmrsStruct->entries; ++pmrIndex) {
        stivale2_pmr pmr = pmrsStruct->pmrs[pmrIndex];
        for (uint64_t pmrVirtAddr = pmr.base; pmrVirtAddr < pmr.base + pmr.length; pmrVirtAddr += 0x1000) {
            uint64_t physAddr = kernelBaseAddrStruct->physical_base_address + (pmrVirtAddr - kernelBaseAddrStruct->virtual_base_address);
            GlobalPageTableManager.MapMemory((void *)(pmrVirtAddr), (void *)(physAddr));
        }
    }

    HuskyStandardOutput.kprint("\nINSERTING INTO CR3\n");
    __asm__ __volatile__("mov %0, %%cr3"
                         :
                         : "r"(PML4Phys));
    HuskyStandardOutput.kprint("INSERTED\n");

    HuskyStandardOutput.kprint("Setting up GDT.\n");
   // makeGDT();

   HuskyStandardOutput.kprint("Initialized");

    for (;;) {
        asm("hlt");
    }
}

Kernel hKernel;

extern "C" void _start(struct stivale2_struct *stivale2_struct) {
    hKernel.KernelStart(stivale2_struct);
}