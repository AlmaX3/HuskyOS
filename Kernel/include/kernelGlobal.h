#pragma once
#include <stivale2.h>
#include <stddef.h>
#include <terminal.h>
#include "string.h"
#include <gdt.h>
#include <hkStdio.h>
#include "memory.h"
#include <pageFrameAllocator.h>
#include <PageMapIndexer.h>
#include <PageTableManager.h>
#include <paging.h>
#include "heap.h"
#include "idt.h"
#include "isr.h"
#include <keyboard.h>
#include <pit.h>
#include "time.h"
#include <serial.h>
#include <vfs.h>
#include <ext2.h>
class Kernel {
public:
    void KernelStart(struct stivale2_struct *stivale2_struct);
    void makeGDT();
    void makeIDT();
    stivale2_tag *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id);
};
extern Kernel hKernel;
extern "C" void cpu_init();