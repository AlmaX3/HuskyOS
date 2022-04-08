[bits 64]
global LoadGDT
LoadGDT:
    lgdt    [rdi]	; load GDT -> rdi is the first argument passed
			; _load_gdt_and_tss_asm(struct GDT_Pointer *ptr)

    mov	    ax, 0x40	; 0x40 is the TSS segment 
    ltr	    ax		; load TSS
    
    mov	    ax,	0x10	; 0x10 is the kernel data segment
    mov	    ds, ax	; load kernel data segment (into data segment registers)
    mov	    es, ax
    mov	    fs, ax
    mov	    gs, ax
    mov	    ss, ax
    
    pop	    rdi		; pop return address
    mov	    rax, 0x08	; 0x08 is the kernel code segment
    push    rax		; push the kernel code segment
    push    rdi		; push return address
    retfq		; far return