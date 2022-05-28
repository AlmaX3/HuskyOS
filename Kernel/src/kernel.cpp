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
    gdt_init();
}

void Kernel::makeIDT() {
    interrupts_init();
}

void Kernel::KernelStart(struct stivale2_struct *stivale2_struct) {
    cpu_init();
    struct stivale2_struct_tag_terminal *term_str_tag;
    term_str_tag = (stivale2_struct_tag_terminal *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);
    TextMode.initializeTextMode(term_str_tag);
    HuskyStandardOutput.kprint("Husky Kernel Started!\n");

    initSerial();

    debug("Started Husky Kernel.\n");
    struct stivale2_struct_tag_memmap *memmap_str_tag;
    memmap_str_tag = (stivale2_struct_tag_memmap *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadMemoryMap(stivale2_struct);

    uint64_t kernel_size = (uint64_t)&end - (uint64_t)&_KernelStart;
    uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernel_pages);

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

    stivale2_struct_tag_kernel_base_address *kernelBaseAddrStruct = (stivale2_struct_tag_kernel_base_address *)(stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID));
    stivale2_struct_tag_pmrs *pmrsStruct = (stivale2_struct_tag_pmrs *)(stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_PMRS_ID));
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

    initializeHeap((void *)0x00001000000, 0x10);

    stivale2_struct_tag_framebuffer *framebuffer = (stivale2_struct_tag_framebuffer *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    GfxMode.initializeFramebuffer(framebuffer);

    HuskyStandardOutput.statuslog(MAGENTA, "Framebuffer", "initialized.\n");
    HuskyStandardOutput.statuslog(MAGENTA, "Framebuffer", "width: %dpx\n", framebuffer->framebuffer_width);
    HuskyStandardOutput.statuslog(MAGENTA, "Framebuffer", "height: %dpx\n", framebuffer->framebuffer_height);
    HuskyStandardOutput.statuslog(MAGENTA, "Framebuffer", "pitch: %d\n", framebuffer->framebuffer_pitch);
    debug("Framebuffer with the size of %lldx%lld with pitch %lld has started.\n", framebuffer->framebuffer_width, framebuffer->framebuffer_height, framebuffer->framebuffer_pitch);

    HuskyStandardOutput.statuslog(CYAN, "Memory", "Free mem: %lld kb\n", GlobalAllocator.GetFreeRAM() / 1024);
    HuskyStandardOutput.statuslog(CYAN, "Memory", "Used mem: %lld kb\n", GlobalAllocator.GetUsedRAM() / 1024);
    HuskyStandardOutput.statuslog(CYAN, "Memory", "Reserved mem: %lld kb\n", GlobalAllocator.GetReservedRAM() / 1024);
    HuskyStandardOutput.statuslog(CYAN, "Memory", "Usage percentage: %lld%%\n", (GlobalAllocator.GetUsedRAM() + GlobalAllocator.GetReservedRAM() / 1024) / (MemFunc.GetAllFreeMemory(memmap_str_tag) / 1024));
    makeGDT();
    makeIDT();
    timer_install();
    EnableKeyboard();

    stivale2_struct_tag_modules *modules = (stivale2_struct_tag_modules *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MODULES_ID);
    uint64_t ramdisk = modules->modules[0].begin;
    uint64_t ramdisk_top = modules->modules[0].end;

    if (ramdisk) {
        initrd_mount(ramdisk, ramdisk_top);
        HuskyStandardOutput.kprint("Attempting to dump license...\n");
        fs_node_t *fsnode = finddir_fs(fs_root, "license.txt");
        if (fsnode) {
            uint8_t *buf = (uint8_t *)malloc(1024);
            uint32_t sz = read_fs(fsnode, 0, 1024, buf);
            //HuskyStandardOutput.kprint("%s\n", buf);
            debug("%s\n", buf);
        } else {
            HuskyStandardOutput.kprint("Failed to dump license!\n");
        }
    }

    DateTime *dt;
    GetTime(dt);
    HuskyStandardOutput.kprint("Booted at ");
    FormatTime(dt);
    HuskyStandardOutput.kprint("\n");
    debug("[%lld.%lld] Booted at: %lld-%lld-%lldT%lld:%lld:%lld\n", getUptime(), getUpSubtime(), dt->year, dt->month, dt->day, dt->hour, dt->minute, dt->second);

    stivale2_struct_tag_rsdp *rsdp = (stivale2_struct_tag_rsdp *)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_RSDP_ID);
    debug("RSDP FOUND AT %p\n", rsdp->rsdp);
    PrepareACPI(rsdp);

    for (size_t i = 0; i < PCIDevices.Length; i++) {
       
        switch (PCIDevices[i]->Class) /*MASS STORAGE DEVICE*/ {
        case 0x1:
            switch (PCIDevices[i]->SubClass) /*SERIAL ATA CONTROLLER*/ {
            case 0x6:
                switch (PCIDevices[i]->ProgramIF) /*AHCI 1.0*/ {
                case 0x1:
                    new AHCIDriver(PCIDevices[i]);
                    break;
                }
                break;
            }
        }
    }

    setlimit(0, cursor.y / GfxMode.charHeight);
    activate_keyboard_processing();

    while (1) {
    }

    asm("hlt");
}

Kernel hKernel;

extern "C" void _start(struct stivale2_struct *stivale2_struct) {
    hKernel.KernelStart(stivale2_struct);
}