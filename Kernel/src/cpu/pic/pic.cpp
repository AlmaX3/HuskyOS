#include <pic.h>
#include "../../../include/string.h"

void pic_disable(void)
{
    outportb(PIC2_DATA, 0xFF);
    outportb(PIC1_DATA, 0xFF);
}

void pic_remap(void)
{
    uint8_t mask1 = inportb(PIC1_DATA);
    uint8_t mask2 = inportb(PIC2_DATA);


    outportb(PIC1_COMMAND, 0x11);
    outportb(PIC2_COMMAND, 0x11);
    io_wait();


    outportb(PIC1_DATA, 0x20);
    outportb(PIC2_DATA, 0x28);
    io_wait();

    outportb(PIC1_DATA, 0x04);
    outportb(PIC2_DATA, 0x02);
    io_wait();

    outportb(PIC1_DATA, 0x01);
    outportb(PIC2_DATA, 0x01);
    io_wait();

    outportb(PIC1_DATA, 0x00);
    outportb(PIC2_DATA, 0x00);
    io_wait();


    outportb(PIC1_DATA, ICW4_8086);
    io_wait();
    outportb(PIC2_DATA, ICW4_8086);
    io_wait();


    outportb(PIC1_DATA, mask1);
    outportb(PIC2_DATA, mask2);
}

void pic_set_mask(uint8_t irq_line)
{
    uint16_t port;
    uint8_t value;

    if (irq_line < 8)
        port = PIC1_DATA;

    else
    {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    value = inportb(port) | (1 << irq_line);
    outportb(port, value);
}
void pic_clear_mask(uint8_t irq_line)
{
    uint16_t port;
    uint8_t value;

    if (irq_line < 8)
        port = PIC1_DATA;

    else
    {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    value = inportb(port) & ~(1 << irq_line);
    outportb(port, value);
}

void pic_signal_EOI(uint64_t isr_number)
{
    if (isr_number >= 40)				
        outportb(PIC2_COMMAND, 0x20);

    outportb(PIC1_COMMAND, 0x20);		
}