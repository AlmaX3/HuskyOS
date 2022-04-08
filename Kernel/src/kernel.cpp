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

stivale2_tag *Kernel::stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (stivale2_tag *)stivale2_struct->tags;
    for (;;) {
        if (current_tag == nullptr) {
            return nullptr;
        }

        if (current_tag->identifier == id) {
            return current_tag;
        }

        current_tag = (stivale2_tag *)current_tag->next;
    }
}

extern uint64_t _KernelStart;
extern uint64_t end;

void Kernel::makeGDT() {
    gdt_init();
    HuskyStandardOutput.kprint("[ GDT STATUS ] INITIALIZED\n");
}

#define GB 0x40000000UL

void Kernel::KernelStart(struct stivale2_struct *stivale2_struct) {
    struct stivale2_struct_tag_terminal *term_str_tag;
    term_str_tag = (stivale2_struct_tag_terminal *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);
    TextMode.initializeTextMode(term_str_tag);

    HuskyStandardOutput.kprint("Husky Kernel Started!\n");

    struct stivale2_struct_tag_memmap *memmap_str_tag;
    memmap_str_tag = (stivale2_struct_tag_memmap *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadMemoryMap(memmap_str_tag);

    uint64_t kernel_size = (uint64_t)&end - (uint64_t)&_KernelStart;
    uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernel_pages);
    HuskyStandardOutput.kprint("TESZHT");

    PageTable *PML4 = (PageTable *)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    GlobalPageTableManager.MapMemory((void *)PML4, (void *)PML4);

    GlobalPageTableManager = PageTableManager(PML4);

    for (uint64_t t = 0; t < MemFunc.GetAllFreeMemory(memmap_str_tag); t += 0x1000) {
        GlobalPageTableManager.MapMemory((void *)t, (void *)t);
    }

    stivale2_struct_tag_framebuffer *framebuffer = (stivale2_struct_tag_framebuffer *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    uint64_t fbBase = (uint64_t)framebuffer->framebuffer_addr;
    uint64_t fbSize = (framebuffer->framebuffer_width * framebuffer->framebuffer_height * framebuffer->framebuffer_bpp) + 0x1000;
    GlobalAllocator.LockPages((void *)fbBase, fbSize / 0x1000 + 1);
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096) {
        GlobalPageTableManager.MapMemory((void *)t, (void *)t);
    }

    HuskyStandardOutput.kprint("TESZHT\n");

    for (uint64_t i = 0; i < 512; i++) {
        PageDirectoryEntry entry = GlobalPageTableManager.PML4->Entries[i];
        HuskyStandardOutput.kprint("%llu  ", entry.Value);
    }

    HuskyStandardOutput.kprint("\nGETTING MEM INFO.\n");
    HuskyStandardOutput.kprint("Free: %llu kb\n", GlobalAllocator.GetFreeRAM() / 1024);
    HuskyStandardOutput.kprint("Used: %llu kb\n", GlobalAllocator.GetUsedRAM() / 1024);
    HuskyStandardOutput.kprint("Reserved: %llu kb\n", GlobalAllocator.GetReservedRAM() / 1024);

    HuskyStandardOutput.kprint("\nINSERTING INTO CR0\n");
    asm("mov %0, %%cr3"
        :
        : "r"(PML4));
    HuskyStandardOutput.kprint("INSERTED\n");
    // makeGDT();

    HuskyStandardOutput.kprint("$ [ root in / ] >");
    for (;;) {
        asm("hlt");
    }
}

Kernel hKernel;

extern "C" void _start(struct stivale2_struct *stivale2_struct) {
    hKernel.KernelStart(stivale2_struct);
}