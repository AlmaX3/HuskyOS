#include "../../../include/string.h"
#include <hkStdio.h>
#include <isr.h>
#include <pageFrameAllocator.h>
#include <pic.h>
#include <serial.h>
#include <terminal.h>

idt_t idtr;

// MAKE INTERRUPTS. ⚠

char *interruptNames[] = {
    "<!>DBZ<!>   |   DIVIDE BY ZERO EXCEPTION.",
    "<!>DBG<!>   |   DEBUG EXCEPTION.",
    "<!>NMI<!>   |   NON-MASKABLE INTERRUPT EXCEPTION.",
    "<!>BRP<!>   |   BREAKPOINT EXCEPTION.",
    "<!>OFW<!>   |   OVERFLOW EXCEPTION.",
    "<!>BNR<!>   |   BOUND RANGE EXCEPTION.",
    "<!>IVO<!>   |   INVALID OPCODE EXCEPTION.",
    "<!>DNA<!>   |   DEVICE NOT AVAILABLE EXCEPTION.",
    "<!>DBF<!>   |   DOUBLE FAULT EXCEPTION.",
    "<!>CSO<!>   |   COPROCESSOR SEGMENT OVERRUN EXCEPTION.",
    "<!>ITS<!>   |   INVALID TSS EXCEPTION.",
    "<!>SNP<!>   |   SEGMENT NOT PRESENT EXCEPTION.",
    "<!>STF<!>   |   STACK FAULT EXCEPTION.",
    "<!>GRP<!>   |   GENERAL PROTECTION EXCEPTION.",
    "<!>PGF<!>   |   PAGE FAULT EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>XFP<!>   |   X87 FLOATING EXCEPTION.",
    "<!>ALC<!>   |   ALIGNMENT CHECK EXCEPTION.",
    "<!>MCC<!>   |   MACHINE CHECK EXCEPTION.",
    "<!>SFP<!>   |   SIMD FLOATING POINT EXCEPTION.",
    "<!>SEH<!>   |   SECURITY-SENSITIVE EVENT IN HOST EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
    "<!>RSV<!>   |   RESERVED EXCEPTION.",
};

char *get_interrupt_name(int interrupt_number) {
    return interruptNames[interrupt_number];
}

void set_idt_gate(void *handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector) {
    idt_desc_entry_t *interrupt = (idt_desc_entry_t *)(idtr.offset + entry_offset * sizeof(idt_desc_entry_t));
    interrupt->set_offset((uint64_t)handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
    interrupt->ist = 0;
}

void irq_remap(void) {
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
    IRQ_RES;
}

void interrupts_init() {
    idtr.limit = 0x0FFF;
    idtr.offset = (uint64_t)GlobalAllocator.RequestPage();

    set_idt_gate((void *)intr_stub_0, 0, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_1, 1, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_2, 2, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_3, 3, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_4, 4, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_5, 5, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_6, 6, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_7, 7, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_8, 8, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_9, 9, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_10, 10, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_11, 11, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_12, 12, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_13, 13, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_14, 14, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_15, 15, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_16, 16, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_17, 17, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_18, 18, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_19, 19, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_20, 20, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_21, 21, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_22, 22, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_23, 23, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_24, 24, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_25, 25, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_26, 26, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_27, 27, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_28, 28, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_29, 29, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_30, 30, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_31, 31, idt_ta_interrupt_gate, 0x08);
    

    asm("lidt %0"
        :
        : "m"(idtr));
    
    IRQ_RES;

    pic_remap();

    set_idt_gate((void *)intr_stub_32, 32, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_33, 33, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_34, 34, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_35, 35, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_36, 36, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_37, 37, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_38, 38, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_39, 39, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_40, 40, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_41, 41, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_42, 42, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_43, 43, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_44, 44, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_45, 45, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_46, 46, idt_ta_interrupt_gate, 0x08);
    set_idt_gate((void *)intr_stub_47, 47, idt_ta_interrupt_gate, 0x08);

    set_idt_gate((void *)intr_stub_255, 255, idt_ta_interrupt_gate, 0x08);
    pic_set_mask(0);
    pic_set_mask(1);
    
    HuskyStandardOutput.statuslog(BLUE, "IDT", "Initialized\n");
}



void irq_check(int irq_no) {
    
    if(irq_no >= 12) {
        outportb(0xA0, 0x20);
    }
    outportb(0x20, 0x20);
}


interrupt_handler *handlers[256];
intr_handler_ptr static_handlers[256];

extern "C" void intr_common_handler_c(s_registers *regs) {
    intr_handler_ptr handler = static_handlers[regs->interrupt_number - 32];
    
    if (regs->interrupt_number <= 0x1f) {
        HuskyStandardOutput.panic(regs, "If this is randomly appearing to you, and you don't know what causes this, please message Alma on Github (https://github.com/AlmaX3), so that he can look into it.\n");
    }

    if (regs->interrupt_number == 0xff) {
        HuskyStandardOutput.kprint("HALTING KERNEL.\n");
        while (true) {
            __asm__ __volatile__("cli; hlt");
        }
    }

    if (regs->interrupt_number >= 0x20 && regs->interrupt_number <= 0x2f) {
        if (handlers[regs->interrupt_number] != NULL) {
            handlers[regs->interrupt_number]->handle();
        }
        if (handler) {
            handler(regs);
        }

        pic_signal_EOI(regs->interrupt_number);
    }

}

interrupt_handler::interrupt_handler(uint8_t int_num) {
    this->int_num = int_num;
    handlers[this->int_num] = this;
}
interrupt_handler::~interrupt_handler() {
    if (handlers[this->int_num] == this) {
        handlers[this->int_num] = NULL;
    }
}

void interrupt_handler::handle() {
}

void register_interrupt_handler(uint8_t intr, intr_handler_ptr handler) {
    static_handlers[intr] = handler;
}
