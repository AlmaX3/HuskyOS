#include <gdt.h>


static struct TSS	    tss;
static struct GDT	    gdt;
static struct GDT_Pointer   gdt_pointer;

// set the arguments for a given segment
void create_descriptors(void)
{
    // 0x00: null
    gdt.null.limit_15_0				    = 0;
    gdt.null.base_15_0				    = 0;
    gdt.null.base_23_16				    = 0;
    gdt.null.type				    = 0x00;
    gdt.null.limit_19_16_and_flags		    = 0x00;
    gdt.null.base_31_24				    = 0;

    // 0x08: kernel code (kernel base selector)
    gdt.kernel_code.limit_15_0			    = 0;
    gdt.kernel_code.base_15_0			    = 0;
    gdt.kernel_code.base_23_16			    = 0;
    gdt.kernel_code.type			    = 0x9A;
    gdt.kernel_code.limit_19_16_and_flags	    = 0xA0;
    gdt.kernel_code.base_31_24			    = 0;

    // 0x10: kernel data
    gdt.kernel_data.limit_15_0			    = 0;
    gdt.kernel_data.base_15_0			    = 0;
    gdt.kernel_data.base_23_16			    = 0;
    gdt.kernel_data.type			    = 0x92;
    gdt.kernel_data.limit_19_16_and_flags	    = 0xA0;
    gdt.kernel_data.base_31_24			    = 0;

    // 0x18: null (user base selector)
    gdt.null2.limit_15_0			    = 0;
    gdt.null2.base_15_0	    			    = 0;
    gdt.null2.base_23_16    			    = 0;
    gdt.null2.type				    = 0x00;
    gdt.null2.limit_19_16_and_flags		    = 0x00;
    gdt.null2.base_31_24			    = 0;

    // 0x20: user data
    gdt.user_data.limit_15_0			    = 0;
    gdt.user_data.base_15_0			    = 0;
    gdt.user_data.base_23_16			    = 0;
    gdt.user_data.type				    = 0x92;
    gdt.user_data.limit_19_16_and_flags		    = 0xA0;
    gdt.user_data.base_31_24			    = 0;

    // 0x28: user code
    gdt.user_code.limit_15_0			    = 0;
    gdt.user_code.base_15_0			    = 0;
    gdt.user_code.base_23_16			    = 0;
    gdt.user_code.type				    = 0x9A;
    gdt.user_code.limit_19_16_and_flags		    = 0xA0;
    gdt.user_code.base_31_24			    = 0;

    // 0x30: ovmf data
    gdt.ovmf_data.limit_15_0			    = 0;
    gdt.ovmf_data.base_15_0			    = 0;
    gdt.ovmf_data.base_23_16			    = 0;
    gdt.ovmf_data.type				    = 0x92;
    gdt.ovmf_data.limit_19_16_and_flags		    = 0xA0;
    gdt.ovmf_data.base_31_24			    = 0;

    // 0x38: ovmf code
    gdt.ovmf_code.limit_15_0			    = 0;
    gdt.ovmf_code.base_15_0			    = 0;
    gdt.ovmf_code.base_23_16			    = 0;
    gdt.ovmf_code.type				    = 0x9A;
    gdt.ovmf_code.limit_19_16_and_flags		    = 0xA0;
    gdt.ovmf_code.base_31_24			    = 0;

    // 0x40: tss low
    gdt.tss_low.limit_15_0			    = 0;
    gdt.tss_low.base_15_0			    = 0;
    gdt.tss_low.base_23_16			    = 0;
    gdt.tss_low.type				    = 0x89;
    gdt.tss_low.limit_19_16_and_flags		    = 0xA0;
    gdt.tss_low.base_31_24			    = 0;

    // 0x48: tss high
    gdt.tss_high.limit_15_0			    = 0;
    gdt.tss_high.base_15_0			    = 0;
    gdt.tss_high.base_23_16			    = 0;
    gdt.tss_high.type				    = 0x00;
    gdt.tss_high.limit_19_16_and_flags		    = 0x00;
    gdt.tss_high.base_31_24			    = 0;
}

// create descriptors and load GDT and TSS
void gdt_init(void)
{
    create_descriptors();

    // memzero the TSS
    for (uint64_t i = 0; i < sizeof(tss); i++)
        ((uint8_t *)(void *)&tss)[i] = 0;

    uint64_t tss_base = ((uint64_t)&tss);

    gdt.tss_low.base_15_0   = tss_base		& 0xffff;
    gdt.tss_low.base_23_16  = (tss_base >> 16)	& 0xff;
    gdt.tss_low.base_31_24  = (tss_base >> 24)	& 0xff;
    gdt.tss_low.limit_15_0  = sizeof(tss);
    gdt.tss_high.limit_15_0 = (tss_base >> 32)	& 0xffff;
    gdt.tss_high.base_15_0  = (tss_base >> 48)	& 0xffff;

    gdt_pointer.limit	= sizeof(gdt) - 1;
    gdt_pointer.base	= (uint64_t)&gdt;

    LoadGDT(&gdt_pointer);
}